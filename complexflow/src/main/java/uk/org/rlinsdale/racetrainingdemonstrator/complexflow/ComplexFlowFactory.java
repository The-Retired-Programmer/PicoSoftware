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
package uk.org.rlinsdale.racetrainingdemonstrator.complexflow;

import org.openide.util.lookup.ServiceProvider;
import uk.org.rlinsdale.racetrainingdemonstrator.core.ScenarioElement;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.FlowElement;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.FlowElementFactory;

/**
 * Factory to create ComplexFlow objects.
 * 
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
@ServiceProvider(service = FlowElementFactory.class)
public class ComplexFlowFactory extends FlowElementFactory {

    @Override
    public FlowElement createInstance(String instancename, ScenarioElement scenario) {
        return new ComplexFlow(instancename, scenario);
    }

    @Override
    public String getName() {
        return "complexflow";
    }
    
    
}
