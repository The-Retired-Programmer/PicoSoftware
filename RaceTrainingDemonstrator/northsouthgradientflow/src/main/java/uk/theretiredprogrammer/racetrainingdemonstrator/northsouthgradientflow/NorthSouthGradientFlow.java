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

import uk.theretiredprogrammer.racetrainingdemonstrator.core.ScenarioElement;
import uk.theretiredprogrammer.racetrainingdemonstrator.complexflow.ComplexFlow;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status;
import static uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status.BADVALUE;
import static uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status.OK;

/**
 * The NorthSouthGradientFlow Class - represents a flow with differing
 * parameters (direction and speed) in a north-south direction. Intermediate
 * positions are interpolated to provide the changing flow.
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
public class NorthSouthGradientFlow extends ComplexFlow {

    /**
     * Constructor
     *
     * @param name the name
     * @param scenario the field of play
     */
    public NorthSouthGradientFlow(String name, ScenarioElement scenario) {
        super(name, scenario);
        double x = scenario.getWest();
        double y = scenario.getSouth();
        southeast.x = x;
        southeast.y = y;
        southwest.x = x;
        southwest.y = y;
        y = scenario.getNorth();
        northeast.x = x;
        northeast.y = y;
        northwest.x = x;
        northwest.y = y;
    }

    @Override
    protected Status setParameter(KeyPair kp) {
        double y;
        int angle;
        double speed;
        try {
            switch (kp.key) {
                case "northposition":
                    y = Double.parseDouble(kp.value);
                    northwest.y = y;
                    northeast.y = y;
                    return OK;
                case "northfrom":
                    angle = Integer.parseInt(kp.value);
                    northwestFlow.setAngle(angle);
                    northeastFlow.setAngle(angle);
                    return OK;
                case "northspeed":
                    speed = Double.parseDouble(kp.value);
                    northwestFlow.setSpeedKnots(speed);
                    northeastFlow.setSpeedKnots(speed);
                    return OK;
                case "southposition":
                    y = Double.parseDouble(kp.value);
                    southwest.y = y;
                    southeast.y = y;
                    return OK;
                case "southfrom":
                    angle = Integer.parseInt(kp.value);
                    southwestFlow.setAngle(angle);
                    southeastFlow.setAngle(angle);
                    return OK;
                case "southspeed":
                    speed = Double.parseDouble(kp.value);
                    southwestFlow.setSpeedKnots(speed);
                    southeastFlow.setSpeedKnots(speed);
                    return OK;
                default:
                    return super.setParameter(kp);
            }
        } catch (NumberFormatException numberFormatException) {
            return BADVALUE;
        }
    }

    @Override
    protected Status checkParameter(KeyPair kp) {
        try {
            switch (kp.key) {
                case "northposition":
                    Double.parseDouble(kp.value);
                    return OK;
                case "northfrom":
                    Integer.parseInt(kp.value);
                    return OK;
                case "northspeed":
                    Double.parseDouble(kp.value);
                    return OK;
                case "southposition":
                    Double.parseDouble(kp.value);
                    return OK;
                case "southfrom":
                    Integer.parseInt(kp.value);
                    return OK;
                case "southspeed":
                    Double.parseDouble(kp.value);
                    return OK;
                default:
                    return super.checkParameter(kp);
            }
        } catch (NumberFormatException numberFormatException) {
            return BADVALUE;
        }
    }
}
