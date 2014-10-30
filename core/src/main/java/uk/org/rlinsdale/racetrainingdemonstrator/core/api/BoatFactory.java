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

import java.util.Map;
import java.util.logging.Level;
import org.openide.util.Lookup;
import uk.org.rlinsdale.nbpcglibrary.common.Log;
import uk.org.rlinsdale.racetrainingdemonstrator.core.ScenarioElement;
import uk.org.rlinsdale.racetrainingdemonstrator.mark.Mark;

/**
 * The Abstract DisplayableElement Factory Class.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 * @param <B> the class of the element entity created by the factory
 */
public abstract class BoatFactory<B extends Boat> {

    /**
     * createInstance an element
     *
     * @param instancename the objectname
     * @param scenario the scenario to be applied
     * @param wind the wind flow to be applied
     * @param water the water flow to be applied
     * @param marks the map of all marks
     * @return the element
     */
    public abstract B createInstance(String instancename, ScenarioElement scenario, FlowElement wind, FlowElement water, Map<String, Mark> marks);

    /**
     * createInstance the classrefname of this DisplayableElement class.
     *
     * @return the classrefname name
     */
    public abstract String getName();

    /**
     * Create a new Instance of the target class.
     *
     * @param classrefname the classrefname of the target class
     * @param instancename the name to be given to the instance
     * @param scenario the scenario to be applied
     * @param wind the wind flow to be applied
     * @param water the water flow to be applied
     * @param marks the map of all marks
     * @return the new instance of the target class
     */
    public static Boat createInstance(String classrefname, String instancename, ScenarioElement scenario, FlowElement wind, FlowElement water, Map<String, Mark> marks) {
        Log.get("uk.org.rlinsdale.racetrainingdemonstrator").log(Level.FINE, "Creating a new instance {0} of class {1}", new Object[]{
            instancename, classrefname
        });
        for (BoatFactory factory : Lookup.getDefault().lookupAll(BoatFactory.class)) {
            if (factory.getName().equals(classrefname)) {
                return factory.createInstance(instancename, scenario, wind, water, marks);
            }
        }
        return null;
    }

}
