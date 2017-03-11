import bpy
import struct
import json
from os import listdir
from os.path import isfile, join, dirname

from bpy_extras.io_utils import ImportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator, Panel

import re
import os
from collections import namedtuple

bl_info = {
	"name": "Autotester Renderer",
	"description": "Imports test results from json files and renders them",
	"author": "Adrian Röfer",
	"version": (1, 0),
	"blender": (2, 76, 0),
	"location": "View3D > Autotester",
	"warning": "", # used for warning icon and text in addons panel
	"wiki_url": "",
	"tracker_url": "",
	"support": "COMMUNITY",
	"category": "Import-Export"
	}

class View3DPanel:
	bl_space_type = 'VIEW_3D'
	bl_region_type = 'TOOLS'

class AutoTester_PT(View3DPanel, Panel):
	bl_category = "Autotester"
	bl_context = "objectmode"
	bl_label = "Autotester"

	def draw(self, context):
		layout = self.layout

		col = layout.column(align=True)

		col.operator("export.mg_scene_data", text="Export Experiment Scene")
		col.operator("import_tests.test_data", text="Import data")
		col.operator("render.render_paths", text="Render Paths")


class PathReader(Operator, ImportHelper):
	bl_idname = "import_tests.test_data"  # important since its how bpy.ops.import_test.some_data is constructed
	bl_label = "Import Test Data"
	
	materials = ['LineRender_RED']
#        ,'LineRender_DEEPPINK', 
#        'LineRender_GOLD', 
#        'LineRender_GREEN', 
#        'LineRender_LIME', 
#        'LineRender_MAGENTA',
#        'LineRender_ORANGERED', 
#        'LineRender_BLUE', 
#        'LineRender_TEAL']
		
	filename_ext = ".json"

	filter_glob = StringProperty(
			default="*.json",
			options={'HIDDEN'},
			)
	
	def createPath(self, filename, context):
		
		f = open(filename, "rb")
		vertices = []
		vels = []
		vC = 0
		connections = []
		for t in struct.iter_unpack('ffff',f.read()):
			vertices.append((t[0],t[1],t[2]))
			vels.append(t[3])
			if vC > 0:
				connections.append((vC - 1, vC))
			
			vC += 1
		
		f.close()
		name = filename[filename.rfind('/')+1:-5]
		pathName = name[name.rfind('_') + 1:]
		name = pathName + '_' + name[name.find('('):name.rfind(')') + 1]
		objIdx = bpy.data.objects.find(name)
		if objIdx > -1:
			bpy.data.objects[objIdx].select = True
			bpy.ops.object.delete()
		
		meshIdx = bpy.data.curves.find(name)
		if meshIdx > -1:
			bpy.data.curves.remove(bpy.data.curves[meshIdx])
		
		bpy.ops.object.add(type='CURVE', location=(0,0,0))
		obj = context.active_object
		obj.name = name
		obj['path'] = True
		obj.data.dimensions = '3D'

		obj.data.splines.new('BEZIER')
		idx = 0
		for v in vertices:
			obj.data.splines[0].bezier_points[idx].co = v
			obj.data.splines[0].bezier_points[idx].handle_left_type = 'VECTOR'
			obj.data.splines[0].bezier_points[idx].handle_right_type = 'VECTOR'
			
			idx += 1
			if idx < len(vertices):
				obj.data.splines[0].bezier_points.add()

		obj.data.bevel_object = context.scene.objects['PathBevel']
		obj.data.resolution_u = 1
		obj.data.name = name
		
		mat = bpy.data.materials[bpy.data.materials.find(self.materials[self.matIdx])]
		
		obj.data.materials.append(mat)
		bpy.ops.object.mode_set(mode='EDIT')
		bpy.ops.object.mode_set(mode='OBJECT')

		print('Sucessfully imported path '+filename)
		
		self.matIdx = (self.matIdx + 1) % len(self.materials)
		
		return obj

	def execute(self, context):
		json_data = open(self.filepath)
		
		data = json.load(json_data)
		configs = data['configs']
		
		objName = data['file'][data['file'].rfind('/')+1:-4]
		obj = context.scene.objects[objName]
		obj['configs'] = []
		configsP = []
		
		dir = dirname(self.filepath) + '/'
		
		self.matIdx = 0

		for config in configs:
			cPaths = []
			
			for path in config['paths']:
				bpy.ops.object.select_all(action='DESELECT')
				pobj = self.createPath(dir+path['points'], context)
				pobj.parent = obj
				cPaths.append(pobj.name)
				
			configsP.append(cPaths)
			print(cPaths)

		obj['configs'] = configsP
		
		return {'FINISHED'}
	
	
class PathRenderer(Operator):
	bl_idname = "render.render_paths"  # important since its how bpy.ops.import_test.some_data is constructed
	bl_label = "Render Paths Data"    
	
	def execute(self, context):
		scenes = [o for o in context.selected_objects if 'scene' in o]
		
		for s in scenes:
			print('Rendering paths of scene ' + s.name)

			for o in s.children:
				if 'camera' in o:
					camera = o
			
			context.scene.camera = camera
			context.scene.render.resolution_x = camera['width']
			context.scene.render.resolution_y = camera['height']
			
			configs = []
			for c in s['configs']:
				cpaths = []
				
				for pname in c:
					obj = bpy.data.objects[pname]
					obj.hide_render = True
					cpaths.append(obj)
			
				configs.append(cpaths)
				
			for c in configs:
				filename = ""
				for p in c:
					p.hide_render = False
					filename = p.name #p.name[:p.name.rfind('_')-1]
					
					context.scene.render.filepath = '//Test_results/rendered_paths/'+filename+'.png'
					bpy.ops.render.render( write_still=True )

					p.hide_render = True
					
					
		return {'FINISHED'}
	


Node = namedtuple("Node", "name parent children depth")

class CVExporter(Operator):
	bl_idname = "export.mg_scene_data"  # important since its how bpy.ops.import_test.some_data is constructed
	bl_label = "Export Experiment Scene"    
	

	def exportVertexCloud(self, obj):

		if obj.type != 'MESH':
			return
			
		filename = os.path.join(self.basedir, 'scenes/meshes/' + obj.data.name)
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
		
		filename = os.path.join(self.basedir, 'scenes/meshes/' + name)
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
		match = re.search('([A-Za-z0-9_]*)(\.[0-9]*)?', goal.name)
		self.writeAttribute(f, 'name', match.group(1))
		self.writeVector(f, 'pos', goal.location)
		self.closeTag(f)

	def writeStart(self, f, start):
		self.openTag(f, 'start')
		match = re.search('([A-Za-z0-9_]*)(\.[0-9]*)?', start.name)
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
		filename = os.path.join(self.basedir, 'scenes/' + root.name)
		f = open(filename+'.xml', 'w')
		self.openTag(f, 'scene')
		self.writeAttribute(f, 'name', root.name)
		for obj in root.children:
			if 'goal' in obj:
				self.writeGoal(f, obj)
			elif 'start' in obj:
				self.writeStart(f, obj)
			elif 'ignore' in obj:
				pass
			else:
				self.writeObstacle(f, obj)
		
		self.closeTag(f)
		f.close()
		print("Exported scene: ",filename) 


	def execute(self, context):
		self.tags = {}
		self.meshes = []
		# export to blend file location
		self.basedir = os.path.dirname(bpy.data.filepath)
		self.currentNode = None

		if not self.basedir:
			raise Exception("Blend file is not saved")

		scene = context.scene

		obj_active = scene.objects.active
		selection = context.selected_objects

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
	
		return {'FINISHED'}
	
def register():
	bpy.utils.register_class(PathRenderer)
	bpy.utils.register_class(PathReader)
	bpy.utils.register_class(CVExporter)
	bpy.utils.register_class(AutoTester_PT)


def unregister():
	bpy.utils.unregister_class(PathRenderer)
	bpy.utils.unregister_class(PathReader)
	bpy.utils.unregister_class(CVExporter)
	bpy.utils.unregister_class(AutoTester_PT)

#if __name__ == "__main__":
#    register()