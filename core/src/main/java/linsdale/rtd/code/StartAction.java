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
import javax.swing.AbstractAction;
import linsdale.nbpcg.supportlib.Log;
import org.openide.util.ImageUtilities;

/**
 * The Start Action.
 * 
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public final class StartAction extends AbstractAction {

    private final ScenarioSimulationElement element;

    /**
     * Constructor
     * 
     * @param element the simulation element into which the action icon is to be placed
     */
    public StartAction(ScenarioSimulationElement element) {
        super("Start Simulation", ImageUtilities.loadImageIcon("com/famfamfam/www/silkicons/control_play_blue.png", false));
        this.element = element;
    }

    @Override
    public void actionPerformed(ActionEvent ev) {
        Log.get("linsdale.rtd").finer("Start button pressed");
        element.start();
    }
}
