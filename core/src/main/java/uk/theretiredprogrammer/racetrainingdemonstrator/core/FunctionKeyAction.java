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
package uk.theretiredprogrammer.racetrainingdemonstrator.core;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import org.openide.awt.ActionID;
import org.openide.awt.ActionReference;
import org.openide.awt.ActionReferences;
import org.openide.awt.ActionRegistration;
import org.openide.util.NbBundle.Messages;
import uk.theretiredprogrammer.nbpcglibrary.common.LogBuilder;

/**
 * Function keystroke action - will pass to a defined keystroke action defined
 * by the definition file.
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
@ActionID(category = "File",
        id = "uk.theretiredprogrammer.racetrainingdemonstrator.FunctionKeyAction")
@ActionRegistration(displayName = "#CTL_FunctionKeyAction")
@ActionReferences({
    @ActionReference(path = "Shortcuts", name = "F1"),
    @ActionReference(path = "Shortcuts", name = "F2"),
    @ActionReference(path = "Shortcuts", name = "F3"),
    @ActionReference(path = "Shortcuts", name = "F4"),
    @ActionReference(path = "Shortcuts", name = "F5"),
    @ActionReference(path = "Shortcuts", name = "F6"),
    @ActionReference(path = "Shortcuts", name = "F7"),
    @ActionReference(path = "Shortcuts", name = "F8"),
    @ActionReference(path = "Shortcuts", name = "F9"),
    @ActionReference(path = "Shortcuts", name = "F10"),
    @ActionReference(path = "Shortcuts", name = "F11"),
    @ActionReference(path = "Shortcuts", name = "F12")
})
@Messages("CTL_FunctionKeyAction=Function Key Action")
public final class FunctionKeyAction implements ActionListener {

    @Override
    public void actionPerformed(ActionEvent e) {
        String key = e.getActionCommand();
        LogBuilder.writeLog("racetrainingdemonstrator", this, "actionPerformed", key);
        ScenarioSimulationDisplay simulationdisplayinfocus = ScenarioSimulationDisplay.getSimulationInFocus();
        if (simulationdisplayinfocus != null) {
            simulationdisplayinfocus.keyAction(key);
        }
    }
}
