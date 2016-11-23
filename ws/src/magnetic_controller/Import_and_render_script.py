import bpy
import struct
import json
from os import listdir
from os.path import isfile, join, dirname

from bpy_extras.io_utils import ImportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator, Panel

bl_info = {
    "name": "Autotester Renderer",
    "description": "Imports test results from json files and renders them",
    "author": "John Doe, Jane Doe",
    "version": (1, 0),
    "blender": (2, 76, 0),
    "location": "View3D > Autotester",
    "warning": "", # used for warning icon and text in addons panel
    "wiki_url": "",
    "tracker_url": "",
    "support": "COMMUNITY",
    "category": "Render"
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
                    filename = p.name[:p.name.rfind('_')-1]
                    
                context.scene.render.filepath = '//Test_Results/Paths/'+filename+'.png'
                bpy.ops.render.render( write_still=True )

                for p in c:
                    p.hide_render = True
                    
        return {'FINISHED'}
    
    
    
def register():
    bpy.utils.register_class(PathRenderer)
    bpy.utils.register_class(PathReader)
    bpy.utils.register_class(AutoTester_PT)


def unregister():
    bpy.utils.unregister_class(PathRenderer)
    bpy.utils.unregister_class(PathReader)
    bpy.utils.unregister_class(AutoTester_PT)

#if __name__ == "__main__":
#    register()