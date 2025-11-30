(
    -- Fonction de comparaison pour trier les objets par nom (alphabétique)
    fn compareNames o1 o2 = stricmp o1.name o2.name

    -- Vérification de la sélection
    if selection.count == 0 then (
        messageBox "Veuillez sélectionner des objets." title:"Erreur"
    )
    else (
        -- Demander où enregistrer le fichier texte
        -- Le dossier choisi servira aussi pour les FBX
        local txtPath = getSaveFileName caption:"Enregistrer le fichier texte et les FBX" types:"Fichier Texte (*.txt)|*.txt"
        
        if txtPath != undefined do (
            -- 1. Préparation des données
            -- On récupère le dossier de destination à partir du chemin du fichier texte
            local exportDir = getFilenamePath txtPath
            
            -- On met les objets sélectionnés dans un tableau pour pouvoir les trier
            local objsToSort = for o in selection collect o
            
            -- On trie le tableau par ordre alphabétique
            qsort objsToSort compareNames
            
            -- 2. Création et écriture du fichier texte
            local theFile = createFile txtPath
            
            for obj in objsToSort do (
                -- Formatage des coordonnées avec 3 décimales (ex: 0.123)
                local sx = formattedPrint obj.pos.x format:".3f"
                local sy = formattedPrint obj.pos.y format:".3f"
                local sz = formattedPrint obj.pos.z format:".3f"
                
                -- Écriture dans le fichier
                format "% / % % %\n" obj.name sx sy sz to:theFile
            )
            close theFile
            
            -- 3. Export des fichiers FBX
            -- On sauvegarde la sélection actuelle pour la restaurer à la fin
            local oldSelection = selection as array
            clearSelection() -- On désélectionne tout pour traiter un par un
            
            for obj in objsToSort do (
                select obj -- On sélectionne uniquement l'objet courant
                
                -- Construction du nom du fichier FBX (Dossier + NomObjet + .fbx)
                local fbxName = exportDir + obj.name + ".fbx"
                
                -- Export en FBX
                -- #noPrompt évite la fenêtre de configuration à chaque fichier (utilise les derniers réglages)
                -- selectedOnly:true force l'export de l'objet sélectionné uniquement
                exportFile fbxName #noPrompt selectedOnly:true using:FBXEXP
            )
            
            -- Restauration de la sélection initiale
            select oldSelection
            
            -- Petit message de confirmation et ouverture du dossier
            messageBox ("Export terminé !\nFichier texte et " + (objsToSort.count as string) + " fichiers FBX générés.")
            shellLaunch exportDir ""
        )
    )
)