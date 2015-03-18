/*
 * Copyright (C) 2014-2015 Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
package uk.org.rlinsdale.racetrainingdemonstrator.core;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import org.openide.awt.ActionID;
import org.openide.awt.ActionReference;
import org.openide.awt.ActionReferences;
import org.openide.awt.ActionRegistration;
import org.openide.util.NbBundle.Messages;
import uk.org.rlinsdale.nbpcglibrary.common.LogBuilder;

/**
 * Function keystroke action - will pass to a defined keystroke action defined
 * by the definition file.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
@ActionID(category = "File",
        id = "uk.org.rlinsdale.racetrainingdemonstrator.FunctionKeyAction")
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
