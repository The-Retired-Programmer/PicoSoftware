/*
 * Copyright (C) 2014 Richard Linsdale (richard.linsdale at blueyonder.co.uk)
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
package linsdale.rtd.code;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.logging.Level;
import linsdale.nbpcg.supportlib.Log;
import org.openide.awt.ActionID;
import org.openide.awt.ActionReference;
import org.openide.awt.ActionReferences;
import org.openide.awt.ActionRegistration;
import org.openide.util.NbBundle.Messages;

/**
 * Function keystroke action - will pass to a defined keystroke action defined
 * by the definition file.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
@ActionID(category = "File",
        id = "linsdale.rta.display.FunctionKeyAction")
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
        Log.get("linsdale.rtd").log(Level.FINER, "Key {0} pressed", key);
        ScenarioSimulationElement el = ScenarioSimulationElement.getSimulationInFocus();
        if (el != null) {
            el.keyAction(key);
        }
    }
}
