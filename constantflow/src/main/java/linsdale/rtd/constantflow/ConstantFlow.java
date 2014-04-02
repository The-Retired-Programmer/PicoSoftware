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
package linsdale.rtd.constantflow;

import linsdale.rtd.code.DefFileModel;
import linsdale.nbpcg.supportlib.OutputReporter;
import linsdale.rtd.code.Scenario;
import linsdale.rtd.complexflow.ComplexFlow;

/**
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class ConstantFlow extends ComplexFlow {

    public ConstantFlow(String name, OutputReporter reporter, DefFileModel dfm) {
        super(name, reporter, dfm);
        Scenario scenario = dfm.getScenario();
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
    public int setParameter(String key, String value) {
        try {
            switch (key) {
                case "from":
                    int angle = Integer.parseInt(value);
                    northeastFlow.setAngle(angle);
                    northwestFlow.setAngle(angle);
                    southeastFlow.setAngle(angle);
                    southwestFlow.setAngle(angle);
                    return PARAM_OK;
                case "speed":
                    double speed = Double.parseDouble(value);
                    northeastFlow.setSpeedKnots(speed);
                    northwestFlow.setSpeedKnots(speed);
                    southeastFlow.setSpeedKnots(speed);
                    southwestFlow.setSpeedKnots(speed);
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
                case "from":
                    Integer.parseInt(value);
                    return PARAM_OK;
                case "speed":
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
