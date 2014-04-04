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
package linsdale.rtd.core.api;

import linsdale.rtd.code.DefFileModel;

/**
 * Abstract Class describing a Flow Model. A Flow Model represents variable
 * flows over a area.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
abstract public class FlowModel extends Rtd {

    /**
     * Constructor
     *
     * @param name the flowmodel name
     * @param dfm the definition file datamodel
     */
    public FlowModel(String name, DefFileModel dfm) {
        super(name, dfm);
    }

    /**
     * Get the flow at a point in the flowmodel at the current time.
     *
     * @param pos the point at which the flow is to be calculated
     * @return the flow
     */
    public abstract Flow getFlow(Location pos);

    /**
     * Get the mean value for the flow across the entire flow model.
     *
     * @return the mean flow
     */
    public abstract Flow getMeanFlow();
}
