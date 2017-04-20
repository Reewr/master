import bpy
from mathutils import Vector, Matrix
from math import degrees, sin, pi

PARTS = {
  'Eye':          ( '0b1000000000000000', '0b1011111111111111',   0, False ),
  'Neck':         ( '0b0100000000000000', '0b0011111111111111',   0, False ),
  'Coxa1':        ( '0b0100100000000000', '0b1000011111111111',   0, False ),
  'ThoraxFront':  ( '0b0011000000000000', '0b1000111111111111',   0, False ),
  'Coxa2':        ( '0b0100010000000000', '0b1000101111111111',   0, False ),
  'SternumFront': ( '0b0100000000000000', '0b1011111111111111',   0, False ),
  'Sternum':      ( '0b0100000000000000', '0b1011111111111111',   0, False ),
  'SternumBack':  ( '0b0100000000000000', '0b1011111111111111',   0, False ),
  'Coxa3':        ( '0b0100001000000000', '0b1000110111111111',   0, False ),
  'ThoraxBack':   ( '0b0011000000000000', '0b1000111111111111',   0, False ),
  'Coxa4':        ( '0b0100000100000000', '0b1000111011111111',   0, False ),
  'Hip':          ( '0b0100000000000000', '0b0011111111111111',   0, False ),
  'Abdomin':      ( '0b1000000000000000', '0b1011111111111111',   0, False ),

  'TrochanterL1': ( '0b0010100000000000', '0b1001011111111111',  30, True  ),
  'FemurL1':      ( '0b0010100000000000', '0b1001011111111111',  35, True  ),
  'PatellaL1':    ( '0b0010100000000000', '0b1101011111111111', -45, True  ),
  'TibiaL1':      ( '0b0010100000000000', '0b1101011111111111', -45, True  ),
  'MetatarsusL1': ( '0b0010100000000000', '0b1101011111111111',   0, False ),
  'TarsusL1':     ( '0b0010100000000000', '0b1101011111111111', -20, False ),

  'TrochanterL2': ( '0b0010010000000000', '0b1001101111111111',  10, True  ),
  'FemurL2':      ( '0b0010010000000000', '0b1001101111111111',  35, True  ),
  'PatellaL2':    ( '0b0010010000000000', '0b1101101111111111', -45, True  ),
  'TibiaL2':      ( '0b0010010000000000', '0b1101101111111111', -45, True  ),
  'MetatarsusL2': ( '0b0010010000000000', '0b1101101111111111',   0, False ),
  'TarsusL2':     ( '0b0010010000000000', '0b1101101111111111', -20, False ),

  'TrochanterL3': ( '0b0010001000000000', '0b1001110111111111', -15, True  ),
  'FemurL3':      ( '0b0010001000000000', '0b1001110111111111',  35, True  ),
  'PatellaL3':    ( '0b0010001000000000', '0b1101110111111111', -45, True  ),
  'TibiaL3':      ( '0b0010001000000000', '0b1101110111111111', -45, True  ),
  'MetatarsusL3': ( '0b0010001000000000', '0b1101110111111111',   0, False ),
  'TarsusL3':     ( '0b0010001000000000', '0b1101110111111111', -20, False ),

  'TrochanterL4': ( '0b0010000100000000', '0b1001111011111111', -40, True  ),
  'FemurL4':      ( '0b0010000100000000', '0b1001111011111111',  35, True  ),
  'PatellaL4':    ( '0b0010000100000000', '0b1101111011111111', -45, True  ),
  'TibiaL4':      ( '0b0010000100000000', '0b1101111011111111', -45, True  ),
  'MetatarsusL4': ( '0b0010000100000000', '0b1101111011111111',   0, False ),
  'TarsusL4':     ( '0b0010000100000000', '0b1101111011111111', -20, False ),

  'TrochanterR1': ( '0b0001100000000000', '0b1010011111111111',  30, True  ),
  'FemurR1':      ( '0b0001100000000000', '0b1010011111111111',  35, True  ),
  'PatellaR1':    ( '0b0001100000000000', '0b1110011111111111', -45, True  ),
  'TibiaR1':      ( '0b0001100000000000', '0b1110011111111111', -45, True  ),
  'MetatarsusR1': ( '0b0001100000000000', '0b1110011111111111',   0, False ),
  'TarsusR1':     ( '0b0001100000000000', '0b1110011111111111', -20, False ),

  'TrochanterR2': ( '0b0001010000000000', '0b1010101111111111',  10, True  ),
  'FemurR2':      ( '0b0001010000000000', '0b1010101111111111',  35, True  ),
  'PatellaR2':    ( '0b0001010000000000', '0b1110101111111111', -45, True  ),
  'TibiaR2':      ( '0b0001010000000000', '0b1110101111111111', -45, True  ),
  'MetatarsusR2': ( '0b0001010000000000', '0b1110101111111111',   0, False ),
  'TarsusR2':     ( '0b0001010000000000', '0b1110101111111111', -20, False ),

  'TrochanterR3': ( '0b0001001000000000', '0b1010110111111111', -15, True  ),
  'FemurR3':      ( '0b0001001000000000', '0b1010110111111111',  35, True  ),
  'PatellaR3':    ( '0b0001001000000000', '0b1110110111111111', -45, True  ),
  'TibiaR3':      ( '0b0001001000000000', '0b1110110111111111', -45, True  ),
  'MetatarsusR3': ( '0b0001001000000000', '0b1110110111111111',   0, False ),
  'TarsusR3':     ( '0b0001001000000000', '0b1110110111111111', -20, False ),

  'TrochanterR4': ( '0b0001000100000000', '0b1010111011111111', -40, True  ),
  'FemurR4':      ( '0b0001000100000000', '0b1010111011111111',  35, True  ),
  'PatellaR4':    ( '0b0001000100000000', '0b1110111011111111', -45, True  ),
  'TibiaR4':      ( '0b0001000100000000', '0b1110111011111111', -45, True  ),
  'MetatarsusR4': ( '0b0001000100000000', '0b1110111011111111',   0, False ),
  'TarsusR4':     ( '0b0001000100000000', '0b1110111011111111', -20, False )
  }


def showVector(vec, comment):
  for k,v in vec.items():
    if not v:
      return ''
    if comment:
      return '{ %f, %f, %f }, // %s' % (v.x, v.y, v.z, k)
    else:
      return '{ %f, %f, %f },' % (v.x, v.y, v.z)


def showPart(name):
  xs = [ PARTS[name][0], PARTS[name][1], 'radians(%i)' % PARTS[name][2], 'true' if PARTS[name][3] else 'false' ]
  return '{ "%s", { %s } }' % (name, ', '.join(xs))

def printParts():
  r = ',\n    '.join([ showPart(x) for x in sorted(PARTS) if bpy.data.objects.get(x) ])
  r = '  { ' + r
  r += ' };'
  print('std::map<std::string, Spider::Part> Spider::SPIDER_PARTS =')
  print(r)


def getInputLegCenterNeurons():
  xs = []

  for j in ['L','R']:
    for i in [1,2,3,4]:
      name = j + str(i) + 'Center'
      x = bpy.data.objects.get(name).matrix_world.to_translation()
      xs.append({ name: x })

  return xs;

def getInputLegTipNeurons():
  xs = []

  for j in ['L','R']:
    for i in [1,2,3,4]:
      name = j + str(i) + 'Tip'
      x = bpy.data.objects.get(name).matrix_world.to_translation()
      xs.append({ name: x })

  return xs;


def getInputNeurons(name):
  xs = []

  f = bpy.data.objects.get(name + 'CM_F')
  if f:
    xs.append({ name+'CM_F': f.matrix_world.to_translation() })

  b = bpy.data.objects.get(name + 'CM_B')
  if b:
    xs.append({ name+'CM_B': b.matrix_world.to_translation() })

  u = bpy.data.objects.get(name + 'CM_U')
  if u:
    xs.append({ name+'CM_U': u.matrix_world.to_translation() })

  u = bpy.data.objects.get(name + 'CM_D')
  if u:
    xs.append({ name+'CM_D': u.matrix_world.to_translation() })


  rot  = bpy.data.objects.get(name + 'Rot')
  if rot:
    xs.append({ name+'Rot': rot.matrix_world.to_translation() })

  return xs;

def getHiddenNeurons():
  xs = []

  for x in bpy.data.objects:
    if "Hidden" in x.name:
      xs.append({ x.name: x.matrix_world.to_translation() })

  return xs

def getOutputNeurons(name):
  xs = []

  rot  = bpy.data.objects.get(name + 'Rot')
  if rot:
    xs.append({ name+'Rot': rot.matrix_world.to_translation() })

  return xs

def printNeuronData():
  inn = []
  hid = []
  out = []

  inn += [ getInputLegCenterNeurons() ]
  inn += [ getInputLegTipNeurons() ]

  inn += [ getInputNeurons(x) for x in sorted(PARTS) if bpy.data.objects.get(x) ]
  hid += [ getHiddenNeurons() ]
  out += [ getOutputNeurons(x) for x in sorted(PARTS) if bpy.data.objects.get(x) ]

  # flatten lists
  inn = [ '    ' + showVector(x, True ) for xs in inn for x in xs if len(xs) > 0 ]
  hid = [ '    ' + showVector(x, False) for xs in hid for x in xs if len(xs) > 0 ]
  out = [ '    ' + showVector(x, True ) for xs in out for x in xs if len(xs) > 0 ]

  print('  std::vector<std::vector<double>> inputs{')
  print('\n'.join(inn))
  print('  };')
  print('  std::vector<std::vector<double>> hidden{')
  print('\n'.join(hid))
  print('  };')
  print('  std::vector<std::vector<double>> outputs{')
  print('\n'.join(out))
  print('  };')


def removeNeurons():
  for x in bpy.data.objects:
    if "CM_" in x.name:
      bpy.ops.object.select_all(action='DESELECT')
      x.select = True
      bpy.ops.object.delete()

    if "Rot_" in x.name:
      bpy.ops.object.select_all(action='DESELECT')
      x.select = True
      bpy.ops.object.delete()

    if "Tip_" in x.name:
      bpy.ops.object.select_all(action='DESELECT')
      x.select = True
      bpy.ops.object.delete()

    if "Hidden" in x.name:
      bpy.ops.object.select_all(action='DESELECT')
      x.select = True
      bpy.ops.object.delete()

def makeNeurons():
  for x in bpy.data.objects:
    if "CM" in x.name:
      f = x.copy()
      bpy.context.scene.objects.link(f)
      f.name = x.name + "_F"
      f.matrix_basis[1][3] = 0.1 if "L" in x.name else -0.1

      b = x.copy()
      bpy.context.scene.objects.link(b)
      b.name = x.name + "_B"
      b.matrix_basis[1][3] = -0.1 if "L" in x.name else 0.1

      l = x.copy()
      bpy.context.scene.objects.link(l)
      l.name = x.name + "_L"
      l.matrix_basis[0][3] = -0.1 if "L" in x.name else 0.1

      r = x.copy()
      bpy.context.scene.objects.link(r)
      r.name = x.name + "_R"
      r.matrix_basis[0][3] = 0.1 if "L" in x.name else -0.1

      u = x.copy()
      bpy.context.scene.objects.link(u)
      u.name = x.name + "_U"
      u.matrix_basis[2][3] = 0.1

      d = x.copy()
      bpy.context.scene.objects.link(d)
      d.name = x.name + "_D"
      d.matrix_basis[2][3] = -0.1

def makeHidden():
  for obj in bpy.data.objects:
    if "Tip" in obj.name:

      while True:
        x = obj.matrix_world.to_translation()
        y = obj.parent.matrix_world.to_translation()
        m = y + (y-x) * 0.5
        s = 1.0 if "L" in obj.parent.name else -1.0
        l = max((y-x).length * 0.25, 0.15)
        k = sin(pi/4) * l

        na = obj.parent.name.replace("Anchor", "")
        cp = [ y for y in obj.parent.children if na == y.name ][0]
        cm = [ y for y in cp.children if na + "CM" == y.name ][0]

        c1 = cm.copy()
        bpy.context.scene.objects.link(c1)
        c1.parent = cm
        c1.name = cm.name + "_Hidden_1"
        c1.matrix_basis = Matrix()
        c1.matrix_basis[1][3] = l

        c2 = cm.copy()
        bpy.context.scene.objects.link(c2)
        c2.parent = cm
        c2.name = cm.name + "_Hidden_2"
        c2.matrix_basis = Matrix()
        c2.matrix_basis[1][3] = k
        c2.matrix_basis[2][3] = k * s

        c3 = cm.copy()
        bpy.context.scene.objects.link(c3)
        c3.parent = cm
        c3.name = cm.name + "_Hidden_3"
        c3.matrix_basis = Matrix()
        c3.matrix_basis[2][3] = l * s

        c4 = cm.copy()
        bpy.context.scene.objects.link(c4)
        c4.parent = cm
        c4.name = cm.name + "_Hidden_4"
        c4.matrix_basis = Matrix()
        c4.matrix_basis[1][3] = -k
        c4.matrix_basis[2][3] = k * s

        c5 = cm.copy()
        bpy.context.scene.objects.link(c5)
        c5.parent = cm
        c5.name = cm.name + "_Hidden_5"
        c5.matrix_basis = Matrix()
        c5.matrix_basis[1][3] = -l

        c6 = cm.copy()
        bpy.context.scene.objects.link(c6)
        c6.parent = cm
        c6.name = cm.name + "_Hidden_6"
        c6.matrix_basis = Matrix()
        c6.matrix_basis[1][3] = -k
        c6.matrix_basis[2][3] = -k * s

        c7 = cm.copy()
        bpy.context.scene.objects.link(c7)
        c7.parent = cm
        c7.name = cm.name + "_Hidden_7"
        c7.matrix_basis = Matrix()
        c7.matrix_basis[2][3] = -l * s

        c8 = cm.copy()
        bpy.context.scene.objects.link(c8)
        c8.parent = cm
        c8.name = cm.name + "_Hidden_8"
        c8.matrix_basis = Matrix()
        c8.matrix_basis[1][3] = k
        c8.matrix_basis[2][3] = -k * s



        m1 = obj.parent.copy()
        bpy.context.scene.objects.link(m1)
        m1.parent = obj.parent
        m1.name = obj.parent.name + "_Hidden_1"
        m1.matrix_basis = Matrix()
        m1.matrix_basis[1][3] = l

        m2 = obj.parent.copy()
        bpy.context.scene.objects.link(m2)
        m2.parent = obj.parent
        m2.name = obj.parent.name + "_Hidden_2"
        m2.matrix_basis = Matrix()
        m2.matrix_basis[1][3] = k
        m2.matrix_basis[2][3] = k * s

        m3 = obj.parent.copy()
        bpy.context.scene.objects.link(m3)
        m3.parent = obj.parent
        m3.name = obj.parent.name + "_Hidden_3"
        m3.matrix_basis = Matrix()
        m3.matrix_basis[2][3] = l * s

        m4 = obj.parent.copy()
        bpy.context.scene.objects.link(m4)
        m4.parent = obj.parent
        m4.name = obj.parent.name + "_Hidden_4"
        m4.matrix_basis = Matrix()
        m4.matrix_basis[1][3] = -k
        m4.matrix_basis[2][3] = k * s

        m5 = obj.parent.copy()
        bpy.context.scene.objects.link(m5)
        m5.parent = obj.parent
        m5.name = obj.parent.name + "_Hidden_5"
        m5.matrix_basis = Matrix()
        m5.matrix_basis[1][3] = -l

        m6 = obj.parent.copy()
        bpy.context.scene.objects.link(m6)
        m6.parent = obj.parent
        m6.name = obj.parent.name + "_Hidden_6"
        m6.matrix_basis = Matrix()
        m6.matrix_basis[1][3] = -k
        m6.matrix_basis[2][3] = -k * s

        m7 = obj.parent.copy()
        bpy.context.scene.objects.link(m7)
        m7.parent = obj.parent
        m7.name = obj.parent.name + "_Hidden_7"
        m7.matrix_basis = Matrix()
        m7.matrix_basis[2][3] = -l * s

        m8 = obj.parent.copy()
        bpy.context.scene.objects.link(m8)
        m8.parent = obj.parent
        m8.name = obj.parent.name + "_Hidden_8"
        m8.matrix_basis = Matrix()
        m8.matrix_basis[1][3] = k
        m8.matrix_basis[2][3] = -k * s

        obj = obj.parent
        if "Sternum" in obj.parent.name:
          break


removeNeurons()
makeNeurons()
makeHidden()

printParts()
printNeuronData()

removeNeurons()
