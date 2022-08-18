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
package uk.theretiredprogrammer.racetrainingdemonstrator.constantflow;

import uk.theretiredprogrammer.racetrainingdemonstrator.core.ScenarioElement;
import uk.theretiredprogrammer.racetrainingdemonstrator.complexflow.ComplexFlow;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status;
import static uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status.BADVALUE;
import static uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status.OK;

/**
 * The ConstantFlow Class - represents a flow which is stready in speed and
 * direction across the entire plane.
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
public class ConstantFlow extends ComplexFlow {

    /**
     * Constructor
     * 
     * @param name the instance name of this flow
     * @param scenario the field of play
     */
    public ConstantFlow(String name, ScenarioElement scenario) {
        super(name, scenario);
        double x = scenario.getWest();
        double y = scenario.getSouth();
        northeast.x = x;
        northeast.y = y;
        northwest.x = x;
        northwest.y = y;
        southeast.x = x;
        southeast.y = y;
        southwest.x = x;
        southwest.y = y;
    }

    @Override
    protected Status setParameter(KeyPair kp) {
        try {
            switch (kp.key) {
                case "from":
                    int angle = Integer.parseInt(kp.value);
                    northeastFlow.setAngle(angle);
                    northwestFlow.setAngle(angle);
                    southeastFlow.setAngle(angle);
                    southwestFlow.setAngle(angle);
                    return OK;
                case "speed":
                    double speed = Double.parseDouble(kp.value);
                    northeastFlow.setSpeedKnots(speed);
                    northwestFlow.setSpeedKnots(speed);
                    southeastFlow.setSpeedKnots(speed);
                    southwestFlow.setSpeedKnots(speed);
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
                case "from":
                    Integer.parseInt(kp.value);
                    return OK;
                case "speed":
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
