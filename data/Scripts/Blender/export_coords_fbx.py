bl_info = {
    "name": "Export Coords & FBX Batch",
    "author": "Ton Nom",
    "version": (1, 0),
    "blender": (2, 80, 0),
    "location": "View3D > Sidebar > Export Tools",
    "description": "Exporte coords en TXT et objets en FBX",
    "category": "Import-Export",
}

import bpy
import os
from bpy_extras.io_utils import ExportHelper

# ------------------------------------------------------------------------
# 1. L'OPÉRATEUR (C'est le moteur du script)
# ------------------------------------------------------------------------
class EXPORT_OT_batch_coords_fbx(bpy.types.Operator, ExportHelper):
    """Exporte les coordonnées et les FBX séparés"""
    bl_idname = "export.batch_coords_fbx"
    bl_label = "Exporter Coords & FBX"
    
    # Filtre pour ne proposer que les fichiers txt dans la fenêtre de sauvegarde
    filename_ext = ".txt"
    filter_glob: bpy.props.StringProperty(
        default="*.txt",
        options={'HIDDEN'},
        maxlen=255,
    )

    def execute(self, context):
        # Récupère le chemin choisi par l'utilisateur
        txt_path = self.filepath
        export_dir = os.path.dirname(txt_path)
        
        # Récupérer la sélection
        selected_objects = context.selected_objects
        
        if not selected_objects:
            self.report({'ERROR'}, "Aucune sélection active.")
            return {'CANCELLED'}

        # Tri Alphabétique
        selected_objects.sort(key=lambda obj: obj.name)

        # Écriture du fichier TXT
        try:
            with open(txt_path, 'w', encoding='utf-8') as f:
                for obj in selected_objects:
                    # Coordonnées Monde
                    loc = obj.matrix_world.translation
                    # Formatage
                    line = f"{obj.name} / {loc.x:.3f} {loc.y:.3f} {loc.z:.3f}\n"
                    f.write(line)
        except Exception as e:
            self.report({'ERROR'}, f"Erreur d'écriture : {e}")
            return {'CANCELLED'}

        # Sauvegarde de l'état de la sélection pour la restaurer après
        bpy.ops.object.select_all(action='DESELECT')
        
        # Boucle d'export FBX
        for obj in selected_objects:
            obj.select_set(True)
            context.view_layer.objects.active = obj
            
            fbx_name = f"{obj.name}.fbx"
            fbx_path = os.path.join(export_dir, fbx_name)
            
            # Export FBX standard
            bpy.ops.export_scene.fbx(
                filepath=fbx_path, 
                use_selection=True, 
                axis_forward='-Z', 
                axis_up='Y'
            )
            
            obj.select_set(False)

        # Restauration de la sélection
        for obj in selected_objects:
            obj.select_set(True)

        self.report({'INFO'}, f"Export terminé : {len(selected_objects)} objets.")
        
        # Optionnel : Ouvrir le dossier (Windows uniquement)
        try:
            os.startfile(export_dir)
        except:
            pass # Ignore si on est sur Mac/Linux ou si ça échoue

        return {'FINISHED'}


# ------------------------------------------------------------------------
# 2. LE PANNEAU (L'interface utilisateur dans le menu N)
# ------------------------------------------------------------------------
class VIEW3D_PT_batch_export_panel(bpy.types.Panel):
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "Export Tools" # Nom de l'onglet sur le côté
    bl_label = "Batch Exporter"

    def draw(self, context):
        layout = self.layout
        row = layout.row()
        row.scale_y = 2.0 # Gros bouton
        row.operator("export.batch_coords_fbx", icon='EXPORT')


# ------------------------------------------------------------------------
# 3. ENREGISTREMENT (Pour que Blender charge l'addon)
# ------------------------------------------------------------------------
classes = (
    EXPORT_OT_batch_coords_fbx,
    VIEW3D_PT_batch_export_panel,
)

def register():
    for cls in classes:
        bpy.utils.register_class(cls)

def unregister():
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)

if __name__ == "__main__":
    register()