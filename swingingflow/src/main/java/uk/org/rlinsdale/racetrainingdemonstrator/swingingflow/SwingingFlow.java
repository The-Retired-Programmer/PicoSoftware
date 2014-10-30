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
package uk.org.rlinsdale.racetrainingdemonstrator.swingingflow;

import uk.org.rlinsdale.racetrainingdemonstrator.core.ScenarioElement;
import uk.org.rlinsdale.racetrainingdemonstrator.complexflow.ComplexFlow;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.BADVALUE;
import static uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair.Status.OK;

/**
 * The SwingingFlow Class - represents a flow which oscillates about a mean
 * direction.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class SwingingFlow extends ComplexFlow {

    /**
     * Constructor
     *
     * @param name the name
     * @param scenario the field of play
     */
    public SwingingFlow(String name, ScenarioElement scenario) {
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
                case "meanfrom":
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
                case "meanfrom":
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
