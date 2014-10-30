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

import java.util.logging.Level;
import org.openide.util.Lookup;
import uk.org.rlinsdale.nbpcglibrary.common.Log;
import uk.org.rlinsdale.racetrainingdemonstrator.core.ScenarioElement;

/**
 * The Abstract DisplayableElement Factory Class.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public abstract class FlowElementFactory {

    /**
     * createInstance an element
     *
     * @param instancename the objectname
     * @param scenario the scenario to be applied
     * @return the element
     */
    public abstract FlowElement createInstance(String instancename, ScenarioElement scenario);

    /**
     * createInstance the classrefname of this FlowElement class.
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
     * @return the new instance of the target class
     */
    public static FlowElement createInstance(String classrefname, String instancename, ScenarioElement scenario) {
        Log.get("uk.org.rlinsdale.racetrainingdemonstrator").log(Level.FINE, "Creating a new instance {0} of class {1}", new Object[]{
            instancename, classrefname
        });
        for (FlowElementFactory factory : Lookup.getDefault().lookupAll(FlowElementFactory.class)) {
            if (factory.getName().equals(classrefname)) {
                return factory.createInstance(instancename, scenario);
            }
        }
        return null;
    }
    
    
}
