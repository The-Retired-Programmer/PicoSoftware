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
package uk.org.rlinsdale.racetrainingdemonstrator.northsouthgradientflow;

import uk.org.rlinsdale.racetrainingdemonstrator.core.ScenarioElement;
import uk.org.rlinsdale.racetrainingdemonstrator.complexflow.ComplexFlow;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.BADVALUE;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.OK;

/**
 * The NorthSouthGradientFlow Class - represents a flow with differing
 * parameters (direction and speed) in a north-south direction. Intermediate
 * positions are interpolated to provide the changing flow.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
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
