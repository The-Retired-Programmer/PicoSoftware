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
import uk.theretiredprogrammer.racetrainingdemonstrator.core.ScenarioElement;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.Boat;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.FlowElement;
import uk.theretiredprogrammer.racetrainingdemonstrator.mark.Mark;

/**
 * The Laser 2 Dinghy.
 * 
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
public class Laser2 extends Boat {

    /**
     *Constructor
     * 
     * @param name the name
     * @param scenario the field of play
     * @param wind the wind flow
     * @param water the water flow
     * @param marks the set of marks
     */
    public Laser2(String name, ScenarioElement scenario, FlowElement wind, FlowElement water, Map<String, Mark> marks) {
        super(name, scenario, wind, water, marks);
        super.metrics(4, 1.5, 0.25, 30, 45, 135); // length, width, inertia, maxturninganglepersec, upwindangle, downwindangle 
        super.performance(
                /* boat speed data */
                new double[][]{
                    //0, 40, 45, 60, 75, 90, 110, 120, 150, 180, 190
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 0
                    {-0.3, 0, 1.1, 1.1, 1.5, 2, 2, 2, 1.2, 0.8, 0.8}, // 2
                    {-0.8, 0, 2.5, 2.9, 2.9, 3.8, 3.5, 3, 2.3, 1.8, 1.8}, // 4
                    {-1.0, 0, 3.1, 3.1, 4.1, 5.6, 5, 4.2, 3.2, 2.5, 2.5}, // 6
                    {-1.1, 0, 3.5, 3.5, 4.6, 6.9, 6.5, 5.8, 4.2, 3.6, 3.6}, // 8
                    {-1.2, 0, 3.9, 3.9, 5.1, 10.9, 10.0, 8, 5, 4.5, 4.5}, // 10
                    {-1.5, 0, 4.3, 4.3, 5.7, 12.4, 13, 10, 6, 5, 5}, // 12
                    {-1.5, 0, 5.1, 5.1, 6.5, 13.5, 14.5, 12, 7.5, 5.5, 5.5}, // 14
                    {-1.5, 0, 5.2, 5.2, 7.0, 14, 17, 14, 9, 7.1, 7.1}, // 16
                    {-1.5, 0, 5.3, 5.3, 7.1, 14.5, 18, 15.8, 10.2, 7.8, 7.8}, // 18
                    {-1.5, 0, 5.3, 5.3, 7.1, 15, 19, 17.5, 12, 9, 9}, // 20
                    {-1.5, 0, 5.3, 5.3, 7.1, 15, 19, 17.5, 12, 9, 9} // 100 (backstop)
                },
                /* angular reference points used in boat speed matrix */
                new int[]{0, 40, 45, 60, 75, 90, 110, 120, 150, 180, 190},
                /* wind speed reference points used in boat speed matrix */
                new double[]{0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 100});
    }
}
