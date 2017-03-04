# -*- coding: utf-8 -*-
# Taken from:
# https://github.com/V0idExp/blender-bullet-export
#
# This file is only used in order to assist with exporting physics from
# blender into a format that the game engine can import.
#
# Usage:
#
# Open blender, go to `user preferences` and load the plugin (file)
# from disk.

from contextlib import contextmanager
import bpy
import os.path

bl_info = {
  "name": "Export Bullet data",
  "category": "Import-Export",
  "author": "Ivan Nikolaev <voidexp@gmail.com>",
  "description": "Export objects physics data in .bullet format",
}

def default_filename(context):
  current_path = context.blend_data.filepath
  if current_path:
    return os.path.join(
      os.path.dirname(current_path),
        '{}.bullet'.format(
          os.path.splitext(os.path.basename(current_path))[0]))
  else:
    return os.path.join(os.path.expanduser("~"), 'physics.bullet')

class BulletDataExporter(bpy.types.Operator):
  """Export physics data"""
  bl_idname = "object.export_physics"
  bl_label = "Bullet physics data (.bullet)"
  bl_options = {'REGISTER'}

  filename = bpy.props.StringProperty(
    name="Filename",
    description="Name of the output .bullet file",
    default="",
    subtype='FILE_PATH')

  @contextmanager
  def text_snippet(self, context):
    self.snip = context.blend_data.texts.new('phys_export_snip')
    self.snip.write(
      'import PhysicsConstraints\n'
      'PhysicsConstraints.exportBulletFile("{}")'.format(self.filename))
    yield
    context.blend_data.texts.remove(self.snip)
    self.snip = None

  def execute(self, context):
    prev_engine = context.scene.render.engine or 'BLENDER_RENDER'
    context.scene.render.engine = 'BLENDER_GAME'
    if not context.selected_objects:
      return {'CANCELLED'}

    obj = context.selected_objects[0]

    with self.text_snippet(context):
      # create a trigger
      bpy.ops.logic.sensor_add(type='ALWAYS', name='phys_export_trigger', object=obj.name)
      trigger = obj.game.sensors[-1]

      # create export controller
      bpy.ops.logic.controller_add(type='PYTHON', name='phys_export', object=obj.name)
      export_ctrl = obj.game.controllers[-1]
      export_ctrl.text = self.snip
      trigger.link(export_ctrl)

      # create AND controller
      bpy.ops.logic.controller_add(type='LOGIC_AND', name='phys_export_pass', object=obj.name)
      pass_ctrl = obj.game.controllers[-1]
      trigger.link(pass_ctrl)

      # create QUIT actuator
      bpy.ops.logic.actuator_add(type='GAME', name='phys_export_quit', object=obj.name)
      quit_act = obj.game.actuators[-1]
      quit_act.mode = 'QUIT'
      pass_ctrl.link(actuator=quit_act)

      # run game engine!
      bpy.ops.view3d.game_start()

      # cleanup
      bpy.ops.logic.controller_remove(controller=export_ctrl.name, object=obj.name)
      bpy.ops.logic.controller_remove(controller=pass_ctrl.name, object=obj.name)
      bpy.ops.logic.actuator_remove(actuator=quit_act.name, object=obj.name)
      bpy.ops.logic.sensor_remove(sensor=trigger.name, object=obj.name)

      context.scene.render.engine = prev_engine
      return {'FINISHED'}

  def invoke(self, context, event):
    if not self.filename:
      self.filename = default_filename(context)
    wm = context.window_manager
    return wm.invoke_props_dialog(self)


def menu_func(self, context):
  self.layout.operator(BulletDataExporter.bl_idname)


def register():
  bpy.utils.register_class(BulletDataExporter)
  bpy.types.INFO_MT_file_export.append(menu_func)


def unregister():
  bpy.utils.unregister_class(BulletDataExporter)
  bpy.types.INFO_MT_file_export.remove(menu_func)
