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
package linsdale.rtd.northsouthgradientflow;

import linsdale.rtd.code.DefFileModel;
import linsdale.rtd.code.Scenario;
import linsdale.rtd.complexflow.ComplexFlow;

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
     * @param dfm the definition file data model
     */
    public NorthSouthGradientFlow(String name, DefFileModel dfm) {
        super(name, dfm);
        Scenario scenario = dfm.getScenario();
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
    public int setParameter(String key, String value) {
        double y;
        int angle;
        double speed;
        try {
            switch (key) {
                case "northposition":
                    y = Double.parseDouble(value);
                    northwest.y = y;
                    northeast.y = y;
                    return PARAM_OK;
                case "northfrom":
                    angle = Integer.parseInt(value);
                    northwestFlow.setAngle(angle);
                    northeastFlow.setAngle(angle);
                    return PARAM_OK;
                case "northspeed":
                    speed = Double.parseDouble(value);
                    northwestFlow.setSpeedKnots(speed);
                    northeastFlow.setSpeedKnots(speed);
                    return PARAM_OK;
                case "southposition":
                    y = Double.parseDouble(value);
                    southwest.y = y;
                    southeast.y = y;
                    return PARAM_OK;
                case "southfrom":
                    angle = Integer.parseInt(value);
                    southwestFlow.setAngle(angle);
                    southeastFlow.setAngle(angle);
                    return PARAM_OK;
                case "southspeed":
                    speed = Double.parseDouble(value);
                    southwestFlow.setSpeedKnots(speed);
                    southeastFlow.setSpeedKnots(speed);
                    return PARAM_OK;
                default:
                    return super.setParameter(key, value);
            }
        } catch (NumberFormatException numberFormatException) {
            return PARAM_BADVALUE;
        }
    }

    @Override
    public int checkParameter(String key, String value) {
        try {
            switch (key) {
                case "northposition":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "northfrom":
                    Integer.parseInt(value);
                    return PARAM_OK;
                case "northspeed":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "southposition":
                    Double.parseDouble(value);
                    return PARAM_OK;
                case "southfrom":
                    Integer.parseInt(value);
                    return PARAM_OK;
                case "southspeed":
                    Double.parseDouble(value);
                    return PARAM_OK;
                default:
                    return super.checkParameter(key, value);
            }
        } catch (NumberFormatException numberFormatException) {
            return PARAM_BADVALUE;
        }
    }
}
