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
package uk.theretiredprogrammer.racetrainingdemonstrator.northsouthgradientflow;

import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.FlowElement;
import org.openide.util.lookup.ServiceProvider;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.ScenarioElement;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.FlowElementFactory;

/**
 * A Factory to create NorthSouthGradientFlow Objects
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
@ServiceProvider(service = FlowElementFactory.class)
public class NorthSouthGradientFlowFactory extends FlowElementFactory {

    @Override
    public FlowElement createInstance(String instancename, ScenarioElement scenario) {
        return new NorthSouthGradientFlow(instancename, scenario);
    }

    @Override
    public String getName() {
        return "northsouthgradientflow";
    }
}
