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
package uk.theretiredprogrammer.racetrainingdemonstrator.laser2;

import java.util.Map;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.BoatFactory;
import org.openide.util.lookup.ServiceProvider;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.ScenarioElement;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.FlowElement;
import uk.theretiredprogrammer.racetrainingdemonstrator.mark.Mark;

/**
 * The Laser2 factory.
 * 
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
@ServiceProvider(service = BoatFactory.class)
public class Laser2Factory extends BoatFactory<Laser2> {

    @Override
    public Laser2 createInstance(String instancename, ScenarioElement scenario, FlowElement wind, FlowElement water, Map<String, Mark> marks) {
        return new Laser2(instancename, scenario, wind, water, marks);
    }

    @Override
    public String getName() {
        return "laser2";
    }
}
