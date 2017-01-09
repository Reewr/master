import bpy
from math import degrees
from mathutils import Matrix, Vector

# Rigify helpers
def get_pose_matrix_in_other_space(mat, pose_bone):
    rest = pose_bone.bone.matrix_local.copy()
    rest_inv = rest.inverted()
    if pose_bone.parent:
        par_mat = pose_bone.parent.matrix.copy()
        par_inv = par_mat.inverted()
        par_rest = pose_bone.parent.bone.matrix_local.copy()
    else:
        par_mat = Matrix()
        par_inv = Matrix()
        par_rest = Matrix()

    smat = rest_inv * (par_rest * (par_inv * mat))

    return smat

def get_local_pose_matrix(pose_bone):
    return get_pose_matrix_in_other_space(pose_bone.matrix, pose_bone)

def get_local_rot_pose_vector(pose_bone):
    rot = Matrix.to_euler(get_local_pose_matrix(pose_bone))
    vec = Vector()
    vec.x = degrees(rot.x)
    vec.y = degrees(rot.y)
    vec.z = degrees(rot.z)
    return vec



# Main
def get_leg (name, mask):
    leg = bpy.data.objects[name].pose.bones

    fs = [0]*10

    fs[0] = get_local_rot_pose_vector(leg['Trochanter']).z
    fs[1] = get_local_rot_pose_vector(leg['Femur']).x
    fs[2] = get_local_rot_pose_vector(leg['Patella']).x
    fs[3] = get_local_rot_pose_vector(leg['Tibia']).x
    fs[4] = get_local_rot_pose_vector(leg['Metatarsus']).y
    fs[5] = get_local_rot_pose_vector(leg['Tarsus']).x
    fs[6] = 0

    target = Matrix.to_translation(bpy.data.objects["Target "+name].matrix_world)
    fs[7] = target.x
    fs[8] = target.z
    fs[9] = -target.y

    for i in range (0, 10):
        fs[i] *= -1 if mask[i] == '-' else 1

    return fs

def get_upper_body ():
    body = bpy.data.objects['Upper Body'].pose.bones
    
    fs = [0]*6

    fs[0] = -get_local_rot_pose_vector(body['Sternum']).x
    fs[1] = -get_local_rot_pose_vector(body['Coxa1']).y
    fs[2] = -get_local_rot_pose_vector(body['Thorax']).x
    fs[3] = -get_local_rot_pose_vector(body['Coxa2']).y
    fs[4] = -get_local_rot_pose_vector(body['Neck']).x
    fs[5] = get_local_rot_pose_vector(body['Head']).z

    return fs

def get_lower_body ():
    body = bpy.data.objects['Lower Body'].pose.bones

    fs = [0]*5

    fs[0] = get_local_rot_pose_vector(body['Sternum']).x
    fs[1] = -get_local_rot_pose_vector(body['Coxa1']).y
    fs[2] = -get_local_rot_pose_vector(body['Thorax']).x
    fs[3] = get_local_rot_pose_vector(body['Coxa2']).y
    fs[4] = -get_local_rot_pose_vector(body['Hip']).x - 25

    return fs

def get_main_axis ():
    m = bpy.data.objects['Spider'].matrix_world
    loc = Matrix.to_translation(m)
    
    fs = [0]*6
    
    fs[0] = loc.x
    fs[1] = loc.z
    fs[2] = -loc.y

    rot = Matrix.to_euler(m)
    
    fs[3] = degrees(rot.x);
    fs[4] = degrees(rot.z);
    fs[5] = -degrees(rot.y);
    
    return fs

class Frame:
    def __init__(self, time, data):
        self.time = time
        self.data = data

    def __str__(self):
        s  = '  <frame time="%.6f">\n    ' % self.time
        s += str.join(' ', list(map(str,self.data)))
        s += '\n  </frame>'
        return s

# --------------------------------------------------------
# Main
frame_size = 97
num_frames = 3
frame_step = 40
duration   = 1.0
mirror     = False
static     = False


frames = []
for i in range(0,num_frames*frame_step,frame_step):
    bpy.data.scenes[bpy.context.scene.name].frame_set(i)

    d =  get_main_axis()
    d += get_upper_body()
    d += get_lower_body()

    if mirror:
        d += get_leg('Leg 1', ' ---     -')
        d += get_leg('Leg 0', '----     -')
        d += get_leg('Leg 3', ' ---     -')
        d += get_leg('Leg 2', '----     -')
        d += get_leg('Leg 5', '----     -')
        d += get_leg('Leg 4', ' ---     -')
        d += get_leg('Leg 7', '----     -')
        d += get_leg('Leg 6', ' ---     -')

        d[6] = -d[6]
        d[11] = -d[11]
        d[12] = -d[12]

    else:
        d += get_leg('Leg 0', '----      ')
        d += get_leg('Leg 1', ' ---      ')
        d += get_leg('Leg 2', '----      ')
        d += get_leg('Leg 3', ' ---      ')
        d += get_leg('Leg 4', ' ---      ')
        d += get_leg('Leg 5', '----      ')
        d += get_leg('Leg 6', ' ---      ')
        d += get_leg('Leg 7', '----      ')
    
    if static:
        d[0] = 0;
        d[2] = 0;
        d[3] = 0;
        d[4] = 0;
        d[5] = 0;

    t = (i/frame_step) / (num_frames-1) * duration

    frames.append (Frame (t,d))


print ('<?xml version="1.0" encoding="utf-8"?>')
print ('<anim frames="%d" frame_size="%d">' % (num_frames, frame_size))
for f in frames:
    print (f)
print ('</anim>')

#'''

