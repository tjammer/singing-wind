import os
import bpy
import sys
sys.path.append('.')
sys.path.append('../external/flatbuffers/python')
from Level import Level, Face, Vertex

if __name__ == "__main__":
    print(os.getcwd())
    bpy.ops.wm.open_mainfile(filepath="../assets/raw/test.blend")
    for obj in bpy.data.objects:
        bpy.context.scene.objects.active = obj
        bpy.ops.object.modifier_apply(modifier="Triangulate")
        obdata = obj.data

        print('Vertices:')
        for v in obdata.vertices:
            print('{}. {} {} {}'.format(v.index, v.co.x, v.co.y, v.co.z))

        print('Faces:')
        for f in obdata.polygons:
            print('{}. '.format(f.index), end='')
            for v in f.vertices:
                print('{} '.format(v), end='')
            print() # for newline
