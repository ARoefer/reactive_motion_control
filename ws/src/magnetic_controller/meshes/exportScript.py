# exports each selected object into its own file

import re
import bpy
import os
from collections import namedtuple

Node = namedtuple("Node", "name parent children depth")

class CVExporter():
	def exportVertexCloud(self, obj):

		if obj.type != 'MESH':
			return
			
		filename = os.path.join(self.basedir, obj.data.name)
		f = open(filename+'.vc', 'w')
		
		surfArea = 0
		for p in obj.data.polygons:
			surfArea += p.area
			
		pSize = surfArea / len(obj.data.vertices) * 0.5
		if pSize == 0:
			pSize = 0.1
			
		for v in obj.data.vertices:
			if len(v.groups) == 0:
				f.write('v ')
				for x in range(0,3):
					f.write(str(v.co[x]))
					f.write(' ')
				
				for x in range(0,3):
					f.write(str(v.normal[x]))
					f.write(' ')
				
				f.write(str(pSize))    
				f.write('\n')

		print("written:", filename)

	def exportPhysMesh(self, name):
		if not name in bpy.data.meshes:
			return
		
		filename = os.path.join(self.basedir, name)
		f = open(filename+'.obj', 'w')
		
		mesh = bpy.data.meshes[name]
		
		for v in mesh.vertices:
			f.write('v ')
			for x in range(0,3):
				f.write(str(v.co[x]))
				f.write(' ')

			f.write('\n')

		for p in mesh.polygons:
			f.write('f ')
			for x in range(0,3):
				f.write(str(p.vertices[x]))
				f.write(' ')     

			f.write('\n')       
			

	def openTag(self, f, name):
		depth = 0
		if self.currentNode != None:
			if not self.currentNode.children:
				f.write('>\n')
				
			self.currentNode = Node(self.currentNode.name, self.currentNode.parent, True, self.currentNode.depth)
			depth = self.currentNode.depth + 1 

		f.write('\t'*depth + '<' + name)
		n = Node(name, self.currentNode, False, depth)
		self.currentNode = n

	def closeTag(self, f):
		if self.currentNode.children:
			f.write('\t'*self.currentNode.depth + '</'+self.currentNode.name+'>\n')
		else:
			f.write('/>\n')

		self.currentNode = self.currentNode.parent
		

	def writeAttribute(self, f, name, value):
		f.write(' '+name+'="'+str(value)+'"')

	def writeVector(self, f, prefix, vector):
		suffix = ['X', 'Y', 'Z']
		
		for x in range(0,3):
			self.writeAttribute(f, prefix+suffix[x], vector[x])

	def writeGoal(self, f, goal):
		self.openTag(f, 'goal')
		match = re.search('([A-Za-z0-9]*)(\.[0-9]*)?', goal.name)
		self.writeAttribute(f, 'name', match.group(1))
		self.writeVector(f, 'pos', goal.location)
		self.closeTag(f)

	def writeStart(self, f, start):
		self.openTag(f, 'start')
		match = re.search('([A-Za-z0-9]*)(\.[0-9]*)?', start.name)
		self.writeAttribute(f, 'name', match.group(1))
		self.writeVector(f, 'pos', start.location)
		self.closeTag(f)	

	def writeObstacle(self, f, obj):
		if obj.type != 'MESH':
			return

		if not obj.data.name in self.meshes:
			self.exportVertexCloud(obj)
			self.exportPhysMesh(obj.data.name + '_phys')
			self.meshes.append(obj.data.name)

		self.openTag(f, 'obstacle')
		self.writeAttribute(f, 'name', obj.name)
		self.writeAttribute(f, 'mesh', 'meshes/'+obj.data.name+'.vc')
		if 'dynamic' in obj:
			self.writeAttribute(f, 'static', 0)
		else:
			self.writeAttribute(f, 'static', 1)
			self.openTag(f, 'transform')
			self.writeVector(f, 'pos', obj.location)
			self.writeVector(f, 'rot', obj.rotation_euler)
			self.closeTag(f)

		self.closeTag(f)

	def exportScene(self, root):
		filename = os.path.join(self.basedir, root.name)
		f = open(filename+'.xml', 'w')
		self.openTag(f, 'scene')
		self.writeAttribute(f, 'name', root.name)
		for obj in root.children:
			if 'goal' in obj:
				self.writeGoal(f, obj)
			elif 'start' in obj:
				self.writeStart(f, obj)
			else:
				self.writeObstacle(f, obj)
		
		self.closeTag(f)
		f.close()
		print("Exported scene: ",filename) 


	def exportScenes(self):
		self.tags = {}
		self.meshes = []
		# export to blend file location
		self.basedir = os.path.dirname(bpy.data.filepath)
		self.currentNode = None

		if not self.basedir:
			raise Exception("Blend file is not saved")

		scene = bpy.context.scene

		obj_active = scene.objects.active
		selection = bpy.context.selected_objects

		bpy.ops.object.select_all(action='DESELECT')

		for obj in selection:

			obj.select = True

			# some exporters only use the active object
			scene.objects.active = obj

			## Can be used for multiple format

			if 'scene' in obj:
				self.exportScene(obj)

			obj.select = False


		scene.objects.active = obj_active

		for obj in selection:
			obj.select = True

if __name__ == '__main__':
	exporter = CVExporter()
	exporter.exportScenes()