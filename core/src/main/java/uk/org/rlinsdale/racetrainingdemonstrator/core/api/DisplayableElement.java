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
package uk.org.rlinsdale.racetrainingdemonstrator.core.api;

import java.awt.Graphics2D;
import uk.org.rlinsdale.nbpcglibrary.common.RegisterLog;
import uk.org.rlinsdale.racetrainingdemonstrator.core.AllElements;

/**
 * The abstract class defining the common interface for all simulation entities.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
@RegisterLog("uk.org.rlinsdale.racetrainingdemonstrator")
abstract public class DisplayableElement extends Element {

    /**
     * The definition file data model
     */
    protected AllElements dfm;

    /**
     * Constructor.
     *
     * @param name the entity name
     * @param dfm he definition file data model
     */
    public DisplayableElement(String name, AllElements dfm) {
        super(name);
        this.dfm = dfm;
    }

    /**
     * Finish the simulation.
     */
    public void finish() {
        dfm = null;
    }

    /**
     * Advance the time to the given time.
     *
     * @param time the time
     */
    abstract public void timerAdvance(int time);

    /**
     * Draw the Flow arrows on the display canvas.
     *
     * @param g2D the 2D graphics object
     * @param pixelsPerMetre the scale factor
     */
    abstract public void draw(Graphics2D g2D, double pixelsPerMetre);
}
