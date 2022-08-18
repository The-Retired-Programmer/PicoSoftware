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
package uk.theretiredprogrammer.racetrainingdemonstrator.eastwestgradientflow;

import uk.theretiredprogrammer.racetrainingdemonstrator.core.ScenarioElement;
import uk.theretiredprogrammer.racetrainingdemonstrator.complexflow.ComplexFlow;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status;
import static uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status.BADVALUE;
import static uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status.OK;

/**
 * The EastWestGradientFlow Class - represents a flow with differing parameters
 * (direction and speed) in a east-west direction. Intermediate positions are
 * interpolated to provide the changing flow.
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
public class EastWestGradientFlow extends ComplexFlow {

    /**
     * Constructor
     *
     * @param name the name
     * @param scenario the field of play
     */
    public EastWestGradientFlow(String name, ScenarioElement scenario) {
        super(name, scenario);
        double x = scenario.getWest();
        double y = scenario.getSouth();
        northwest.x = x;
        northwest.y = y;
        southwest.x = x;
        southwest.y = y;
        x = scenario.getEast();
        northeast.x = x;
        northeast.y = y;
        southeast.x = x;
        southeast.y = y;
    }

    @Override
    protected Status setParameter(KeyPair kp) {
        double x;
        int angle;
        double speed;
        try {
            switch (kp.key) {
                case "westposition":
                    x = Double.parseDouble(kp.value);
                    northwest.x = x;
                    southwest.x = x;
                    return OK;
                case "westfrom":
                    angle = Integer.parseInt(kp.value);
                    northwestFlow.setAngle(angle);
                    southwestFlow.setAngle(angle);
                    return OK;
                case "westspeed":
                    speed = Double.parseDouble(kp.value);
                    northwestFlow.setSpeedKnots(speed);
                    southwestFlow.setSpeedKnots(speed);
                    return OK;
                case "eastposition":
                    x = Double.parseDouble(kp.value);
                    northeast.x = x;
                    southeast.x = x;
                    return OK;
                case "eastfrom":
                    angle = Integer.parseInt(kp.value);
                    northeastFlow.setAngle(angle);
                    southeastFlow.setAngle(angle);
                    return OK;
                case "eastspeed":
                    speed = Double.parseDouble(kp.value);
                    northeastFlow.setSpeedKnots(speed);
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
                case "westposition":
                    Double.parseDouble(kp.value);
                    return OK;
                case "westfrom":
                    Integer.parseInt(kp.value);
                    return OK;
                case "westspeed":
                    Double.parseDouble(kp.value);
                    return OK;
                case "eastposition":
                    Double.parseDouble(kp.value);
                    return OK;
                case "eastfrom":
                    Integer.parseInt(kp.value);
                    return OK;
                case "eastspeed":
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
