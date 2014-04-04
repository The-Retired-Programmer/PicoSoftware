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
import linsdale.rtd.core.api.RtdCore;
import linsdale.rtd.core.api.Rtd;

/**
 * The model representing the definition file.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class DefFileModel {

    private final ArrayList<Rtd> rtaobjects = new ArrayList<>();
    private final HashMap<String, DefFileComponentModel> commands = new HashMap<>();
    private final HashMap<String, DefFileComponentModel> keyDefinitions = new HashMap<>();
    private final HashMap<Integer, DefFileComponentModel> futureDefinitions = new HashMap<>();
    private Scenario sc;
    private FlowModel water;
    private FlowModel wind;

    /**
     * Finish the simulation.
     */
    public void finish() {
        water.finish();
        wind.finish();
        rtaobjects.stream().forEach((rta) -> {
            rta.finish();
        });
    }

    /**
     * Get the Scenario
     *
     * @return the scenario
     */
    public Scenario getScenario() {
        return sc;
    }

    /**
     * Get the Wind Flowmodel
     *
     * @return the flowmodel for the wind
     */
    public FlowModel getWind() {
        return wind;
    }

    /**
     * Get the Water Flowmodel
     *
     * @return the flowmodel for the water
     */
    public FlowModel getWater() {
        return water;
    }

    /**
     * load data into the definition file model
     *
     * @param in the inputstream from which the file is read
     * @param errors a string buffer to which error messages can be appended
     */
    public void load(InputStream in, StringBuffer errors) {
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
        createWaterInstance(watercommand, errors);
        // set up the wind object
        DefFileComponentModel windcommand = commands.remove("wind");
        if (windcommand == null) {
            windcommand = new DefFileComponentModel("wind", "constantflow");
            windcommand.addparameter("speed", "10");// need some default wind speed
        }
        createWindInstance(windcommand, errors);
        // setup all the rest of the instances required
        createInstances(errors);
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

    private void createWindInstance(DefFileComponentModel def, StringBuffer errors) {
        wind = (FlowModel) FactoryManager.newInstance(def.getClassname(), def.getInstancename(), this);
        wind.setParameters(def.getParameters(), errors);
    }

    private void createWaterInstance(DefFileComponentModel def, StringBuffer errors) {
        water = (FlowModel) FactoryManager.newInstance(def.getClassname(), def.getInstancename(), this);
        water.setParameters(def.getParameters(), errors);
    }

    private void createInstances(StringBuffer errors) {
        commands.keySet().stream().map((key) -> commands.get(key)).forEach((def) -> {
            Rtd instance = FactoryManager.newInstance(def.getClassname(), def.getInstancename(), this);
            rtaobjects.add(instance);
            instance.setParameters(def.getParameters(), errors);
        });
    }

    private void setParameters(DefFileComponentModel def, StringBuffer errors) {
        String name = def.getInstancename();
        RtdCore instance = getNamedInstance(name);
        if (instance == null) {
            errors.append("instance does not exist (").append(name).append(")\n");
        } else {
            instance.setParameters(def.getParameters(), errors);
        }
    }

    private void checkParameters(DefFileComponentModel def, StringBuffer errors) {
        String name = def.getInstancename();
        RtdCore instance = getNamedInstance(name);
        if (instance == null) {
            errors.append("instance does not exist (").append(name).append(")\n");
        } else {
            instance.checkParameters(def.getParameters(), errors);
        }
    }

    /**
     * Get a named instance (flow, boat, mark etc)
     *
     * @param name the required name
     * @return the instance
     */
    public Rtd getNamedInstance(String name) {
        if (name.equals("wind")) {
            return wind;
        }
        if (name.equals("water")) {
            return water;
        }
        for (Rtd instance : rtaobjects) {
            if (instance.getName().equals(name)) {
                return instance;
            }
        }
        return null;
    }

    /**
     * Advance time.
     *
     * @param time the new time
     */
    public void executeAllTimeAdvance(int time) {
        water.timerAdvance(time);
        wind.timerAdvance(time);
        rtaobjects.stream().forEach((rtaobj) -> {
            rtaobj.timerAdvance(time);
        });
    }

    /**
     * Draw the simulation on the display canvas.
     *
     * @param g2D the 2D graphics object
     */
    public void executeAllDraw(Graphics2D g2D) {
        Double pixelsPerMetre = sc.draw(g2D);
        water.draw(g2D, pixelsPerMetre);
        wind.draw(g2D, pixelsPerMetre);
        rtaobjects.stream().forEach((rtaobj) -> {
            rtaobj.draw(g2D, pixelsPerMetre);
        });
    }

    // the instance definitions - used to create the executable model at time zero
    /**
     * Add a new instance definition
     *
     * @param instancename the name
     * @param def the component definition model
     */
    public void addInstanceDefinition(String instancename, DefFileComponentModel def) {
        commands.put(instancename, def);
    }

    // the key definitions data model - used to change parameters in the executable data model
    /**
     * Add a keystoke definition - this is a mapping of a keystroke to a component model
     *
     * @param keystring the keystroke
     * @param def the component definition model
     */
    public void addkeyDefinition(String keystring, DefFileComponentModel def) {
        keyDefinitions.put(keystring, def);
    }

    private void checkAllKeyDefs(StringBuffer errors) {
        keyDefinitions.keySet().stream().forEach((key) -> {
            checkParameters(keyDefinitions.get(key), errors);
        });
    }

    /**
     * Taken action on keystroke
     * 
     * @param key the keystroke
     */
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
    /**
     * Add a future update to the parameter value(s) at some time
     * 
     * @param time the time
     * @param def the revised component definition model
     */
    public void addFutureDefinition(int time, DefFileComponentModel def) {
        futureDefinitions.put(time, def);
    }

    private void checkAllFutureDefs(StringBuffer errors) {
        futureDefinitions.keySet().stream().forEach((key) -> {
            checkParameters(futureDefinitions.get(key), errors);
        });
    }

    /**
     * Process any future updates of parameter values which have to be made at this particular time.
     * 
     * @param time the time
     */
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
