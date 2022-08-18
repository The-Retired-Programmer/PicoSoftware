/*
 * Copyright 2014-2017 Richard Linsdale.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package uk.theretiredprogrammer.racetrainingdemonstrator.definitionfiletype;

import java.io.IOException;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.ScenarioEditorDisplay;
import org.netbeans.core.spi.multiview.MultiViewElement;
import org.netbeans.core.spi.multiview.text.MultiViewEditorElement;
import org.openide.awt.ActionID;
import org.openide.awt.ActionReference;
import org.openide.awt.ActionReferences;
import org.openide.filesystems.FileObject;
import org.openide.filesystems.MIMEResolver;
import org.openide.loaders.DataObject;
import org.openide.loaders.DataObjectExistsException;
import org.openide.loaders.MultiDataObject;
import org.openide.loaders.MultiFileLoader;
import org.openide.util.Lookup;
import org.openide.util.NbBundle.Messages;
import org.openide.windows.TopComponent;

/**
 * The DataObject for the definition file.
 * 
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
@Messages({
    "LBL_DefFile_LOADER=Definition Files"
})
@MIMEResolver.ExtensionRegistration(
        displayName = "#LBL_DefFile_LOADER",
        mimeType = "text/x-rtd",
        extension = {"rtd", "RTD"})
@DataObject.Registration(
        mimeType = "text/x-rtd",
        iconBase = "uk/theretiredprogrammer/racetrainingdemonstrator/definitionfiletype/shape_flip_horizontal.png",
        displayName = "#LBL_DefFile_LOADER",
        position = 300)
@ActionReferences({
    @ActionReference(
            path = "Loaders/text/x-rtd/Actions",
            id =
            @ActionID(category = "System", id = "org.openide.actions.OpenAction"),
            position = 100,
            separatorAfter = 200),
    @ActionReference(
            path = "Loaders/text/x-rtd/Actions",
            id =
            @ActionID(category = "Edit", id = "org.openide.actions.CutAction"),
            position = 300),
    @ActionReference(
            path = "Loaders/text/x-rtd/Actions",
            id =
            @ActionID(category = "Edit", id = "org.openide.actions.CopyAction"),
            position = 400,
            separatorAfter = 500),
    @ActionReference(
            path = "Loaders/text/x-rtd/Actions",
            id =
            @ActionID(category = "Edit", id = "org.openide.actions.DeleteAction"),
            position = 600),
    @ActionReference(
            path = "Loaders/text/x-rtd/Actions",
            id =
            @ActionID(category = "System", id = "org.openide.actions.RenameAction"),
            position = 700,
            separatorAfter = 800),
    @ActionReference(
            path = "Loaders/text/x-rtd/Actions",
            id =
            @ActionID(category = "System", id = "org.openide.actions.SaveAsTemplateAction"),
            position = 900,
            separatorAfter = 1000),
    @ActionReference(
            path = "Loaders/text/x-rtd/Actions",
            id =
            @ActionID(category = "System", id = "org.openide.actions.FileSystemAction"),
            position = 1100,
            separatorAfter = 1200),
    @ActionReference(
            path = "Loaders/text/x-rtd/Actions",
            id =
            @ActionID(category = "System", id = "org.openide.actions.ToolsAction"),
            position = 1300),
    @ActionReference(
            path = "Loaders/text/x-rtd/Actions",
            id =
            @ActionID(category = "System", id = "org.openide.actions.PropertiesAction"),
            position = 1400)
})
public class DefFileDataObject extends MultiDataObject {

    /**
     * Constructor.
     * 
     * @param pf the file object
     * @param loader the loader
     * @throws DataObjectExistsException if failure
     * @throws IOException if failure
     */
    public DefFileDataObject(FileObject pf, MultiFileLoader loader) throws DataObjectExistsException, IOException {
        super(pf, loader);
        registerEditor("text/x-rtd", true);
    }

    @Override
    protected int associateLookup() {
        return 1;
    }

    /**
     * Create the associated editor element.
     * 
     * @param lkp the top component element
     * @return the editor element
     */
    @MultiViewElement.Registration(
            displayName = "#LBL_DefFile_EDITOR",
            iconBase = "uk/theretiredprogrammer/racetrainingdemonstrator/definitionfiletype/shape_flip_horizontal.png",
            mimeType = "text/x-rtd",
            persistenceType = TopComponent.PERSISTENCE_ONLY_OPENED,
            preferredID = "DefFile",
            position = 1000)
    @Messages("LBL_DefFile_EDITOR=Definition File")
    public static MultiViewEditorElement createEditor(Lookup lkp) {
        return new ScenarioEditorDisplay(lkp);
    }
}
