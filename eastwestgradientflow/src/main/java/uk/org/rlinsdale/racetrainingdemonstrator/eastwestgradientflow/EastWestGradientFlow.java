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
package uk.org.rlinsdale.racetrainingdemonstrator.eastwestgradientflow;

import uk.org.rlinsdale.racetrainingdemonstrator.core.ScenarioElement;
import uk.org.rlinsdale.racetrainingdemonstrator.complexflow.ComplexFlow;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.BADVALUE;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.OK;

/**
 * The EastWestGradientFlow Class - represents a flow with differing parameters
 * (direction and speed) in a east-west direction. Intermediate positions are
 * interpolated to provide the changing flow.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
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
