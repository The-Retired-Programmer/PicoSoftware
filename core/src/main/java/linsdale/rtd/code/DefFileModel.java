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
package linsdale.rtd.code;

import java.awt.Graphics2D;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import linsdale.nbpcg.supportlib.LogicException;
import linsdale.rtd.core.api.FlowModel;
import linsdale.rtd.core.api.RTABaseObject;
import linsdale.rtd.core.api.RTAObject;
import linsdale.nbpcg.supportlib.OutputReporter;

/**
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class DefFileModel {

    private final ArrayList<RTAObject> rtaobjects = new ArrayList<>();
    private final HashMap<String, DefFileComponentModel> commands = new HashMap<>();
    private final HashMap<String, DefFileComponentModel> keyDefinitions = new HashMap<>();
    private final HashMap<Integer, DefFileComponentModel> futureDefinitions = new HashMap<>();
    private Scenario sc;
    private FlowModel water;
    private FlowModel wind;

    public void finish() {
        water.finish();
        wind.finish();
        rtaobjects.stream().forEach((rta) -> {
            rta.finish();
        });
    }

    public Scenario getScenario() {
        return sc;
    }

    public FlowModel getWind() {
        return wind;
    }

    public FlowModel getWater() {
        return water;
    }

    public void load(OutputReporter reporter, InputStream in, StringBuffer errors) {
        Parser parser = new Parser();
        parser.parse(in, this, errors);
        // set up the scenario object
        DefFileComponentModel scenariocommand = commands.remove("scenario");
        if (scenariocommand == null) {
            throw new LogicException("Error - scenario command missing (should be automatically created");
        }
        createScenarioInstance(scenariocommand, errors);
        // set up the water object
        DefFileComponentModel watercommand = commands.remove("water");
        if (watercommand == null) {
            watercommand = new DefFileComponentModel("water", "constantflow"); // null flow is OK
        }
        createWaterInstance(reporter, watercommand, errors);
        // set up the wind object
        DefFileComponentModel windcommand = commands.remove("wind");
        if (windcommand == null) {
            windcommand = new DefFileComponentModel("wind", "constantflow");
            windcommand.addparameter("speed", "10");// need some default wind speed
        }
        createWindInstance(reporter, windcommand, errors);
        // setup all the rest of the instances required
        createInstances(reporter, errors);
        // now go through all the future time objects - checking syntax of parameters
        checkAllFutureDefs(errors);
        // now go through all the function key objects - checking syntax of parameters
        checkAllKeyDefs(errors);
    }

    // the resulting executable objects data model
    private void createScenarioInstance(DefFileComponentModel def, StringBuffer errors) {
        sc = new Scenario();
        sc.setParameters(def.getParameters(), errors);
    }

    private void createWindInstance(OutputReporter reporter, DefFileComponentModel def, StringBuffer errors) {
        wind = (FlowModel) FactoryManager.newInstance(def.getClassname(), def.getInstancename(), reporter, this);
        wind.setParameters(def.getParameters(), errors);
    }

    private void createWaterInstance(OutputReporter reporter, DefFileComponentModel def, StringBuffer errors) {
        water = (FlowModel) FactoryManager.newInstance(def.getClassname(), def.getInstancename(), reporter, this);
        water.setParameters(def.getParameters(), errors);
    }

    private void createInstances(OutputReporter reporter, StringBuffer errors) {
        commands.keySet().stream().map((key) -> commands.get(key)).forEach((def) -> {
            RTAObject instance = FactoryManager.newInstance(def.getClassname(), def.getInstancename(), reporter, this);
            rtaobjects.add(instance);
            instance.setParameters(def.getParameters(), errors);
        });
    }

    private void setParameters(DefFileComponentModel def, StringBuffer errors) {
        String name = def.getInstancename();
        RTABaseObject instance = getNamedInstance(name);
        if (instance == null) {
            errors.append("instance does not exist (").append(name).append(")\n");
        } else {
            instance.setParameters(def.getParameters(), errors);
        }
    }

    private void checkParameters(DefFileComponentModel def, StringBuffer errors) {
        String name = def.getInstancename();
        RTABaseObject instance = getNamedInstance(name);
        if (instance == null) {
            errors.append("instance does not exist (").append(name).append(")\n");
        } else {
            instance.checkParameters(def.getParameters(), errors);
        }
    }

    public RTAObject getNamedInstance(String name) {
        if (name.equals("wind")) {
            return wind;
        }
        if (name.equals("water")) {
            return water;
        }
        for (RTAObject instance : rtaobjects) {
            if (instance.getName().equals(name)) {
                return instance;
            }
        }
        return null;
    }

    public void executeAllTimeAdvance(int time) {
        water.timerAdvance(time);
        wind.timerAdvance(time);
        rtaobjects.stream().forEach((rtaobj) -> {
            rtaobj.timerAdvance(time);
        });
    }

    public void executeAllDraw(Graphics2D g2D) {
        Double pixelsPerMetre = sc.draw(g2D);
        water.draw(g2D, pixelsPerMetre);
        wind.draw(g2D, pixelsPerMetre);
        rtaobjects.stream().forEach((rtaobj) -> {
            rtaobj.draw(g2D, pixelsPerMetre);
        });
    }

    // the instance definitions - used to create the executable model at time zero
    public void addInstanceDefinition(String instancename, DefFileComponentModel def) {
        commands.put(instancename, def);
    }

    // the key definitions data model - used to change parameters in the executable data model
    public void addkeyDefinition(String keystring, DefFileComponentModel def) {
        keyDefinitions.put(keystring, def);
    }

    private void checkAllKeyDefs(StringBuffer errors) {
        keyDefinitions.keySet().stream().forEach((key) -> {
            checkParameters(keyDefinitions.get(key), errors);
        });
    }

    public void processKey(String key) {
        DefFileComponentModel def = keyDefinitions.get(key);
        if (def != null) {
            StringBuffer errs = new StringBuffer();
            setParameters(def, errs);
            if (errs.length() > 0) {
                throw new LogicException("Error when processing key=" + key + "\n" + errs.toString());
            }
        }
    }

    // the future definitions data model - used to change parameters in the executable data model
    public void addFutureDefinition(int time, DefFileComponentModel def) {
        futureDefinitions.put(time, def);
    }

    private void checkAllFutureDefs(StringBuffer errors) {
        futureDefinitions.keySet().stream().forEach((key) -> {
            checkParameters(futureDefinitions.get(key), errors);
        });
    }

    public void processDefinitions(int time) {
        DefFileComponentModel def = futureDefinitions.remove(time);
        if (def != null) {
            StringBuffer errs = new StringBuffer();
            setParameters(def, errs);
            if (errs.length() > 0) {
                throw new LogicException("Error when processing definition at time="
                        + ScenarioSimulationElement.mmssformat(time) + "\n" + errs);
            }
        }
    }
}
