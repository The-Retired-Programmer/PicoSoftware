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
package uk.org.rlinsdale.racetrainingdemonstrator.core;

import java.awt.Graphics2D;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import uk.org.rlinsdale.nbpcglibrary.common.LogicException;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.FlowElement;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.Element;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.DisplayableElement;

/**
 * The model representing the definition file.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class AllElements {

    private final ArrayList<DisplayableElement> elements = new ArrayList<>();
    private final HashMap<String, ElementParameters> commands = new HashMap<>();
    private final HashMap<String, ElementParameters> keyDefinitions = new HashMap<>();
    private final HashMap<Integer, ElementParameters> futureDefinitions = new HashMap<>();
    private ScenarioElement scenario;
    private FlowElement water;
    private FlowElement wind;

    /**
     * Finish the simulation.
     */
    public void finish() {
        water.finish();
        wind.finish();
        elements.stream().forEach((element) -> {
            element.finish();
        });
    }

    /**
     * Get the ScenarioElement
     *
     * @return the scenario
     */
    public ScenarioElement getScenarioElement() {
        return scenario;
    }

    /**
     * Get the Wind Flowmodel
     *
     * @return the flowmodel for the wind
     */
    public FlowElement getWind() {
        return wind;
    }

    /**
     * Get the Water Flowmodel
     *
     * @return the flowmodel for the water
     */
    public FlowElement getWater() {
        return water;
    }

    /**
     * load data from definitions file
     *
     * @param in the inputstream from which the file is read
     * @param errors a string buffer to which error messages can be appended
     */
    public void load(InputStream in, StringBuffer errors) {
        Parser parser = new Parser();
        parser.parse(in, this, errors);
        // set up the scenario object
        ElementParameters parameters = commands.remove("scenario");
        if (parameters == null) {
            throw new LogicException("Error - scenario command missing (should be automatically created");
        }
        createScenarioElement(parameters, errors);
        // set up the water object
        parameters = commands.remove("water");
        if (parameters == null) {
            parameters = new ElementParameters("water", "constantflow"); // null flow is OK
        }
        createWaterElement(parameters, errors);
        // set up the wind object
        parameters = commands.remove("wind");
        if (parameters == null) {
            parameters = new ElementParameters("wind", "constantflow");
            parameters.addparameter("speed", "10");// need some default wind speed
        }
        createWindElement(parameters, errors);
        // setup all the rest of the elements defined
        createElements(errors);
        // now go through all the future time objects - checking syntax of parameters
        checkAllFutureDefs(errors);
        // now go through all the function key objects - checking syntax of parameters
        checkAllKeyDefs(errors);
    }

    // the resulting executable objects data model
    private void createScenarioElement(ElementParameters parameters, StringBuffer errors) {
        scenario = new ScenarioElement();
        scenario.setParameters(parameters.getParameters(), errors);
    }

    private void createWindElement(ElementParameters def, StringBuffer errors) {
        wind = (FlowElement) ElementFactoryManager.get(def.getClassname(), def.getInstancename(), this);
        wind.setParameters(def.getParameters(), errors);
    }

    private void createWaterElement(ElementParameters def, StringBuffer errors) {
        water = (FlowElement) ElementFactoryManager.get(def.getClassname(), def.getInstancename(), this);
        water.setParameters(def.getParameters(), errors);
    }

    private void createElements(StringBuffer errors) {
        commands.values().stream().forEach((parameters) -> {
            DisplayableElement element = ElementFactoryManager.get(parameters.getClassname(), parameters.getInstancename(), this);
            elements.add(element);
            element.setParameters(parameters.getParameters(), errors);
        });
    }

    private void setParameters(ElementParameters parameters, StringBuffer errors) {
        String name = parameters.getInstancename();
        Element element = get(name);
        if (element == null) {
            errors.append("instance does not exist (").append(name).append(")\n");
        } else {
            element.setParameters(parameters.getParameters(), errors);
        }
    }

    private void checkParameters(ElementParameters parameters, StringBuffer errors) {
        String name = parameters.getInstancename();
        Element element = get(name);
        if (element == null) {
            errors.append("instance does not exist (").append(name).append(")\n");
        } else {
            element.checkParameters(parameters.getParameters(), errors);
        }
    }

    /**
     * Get a named element (flow, boat, mark etc)
     *
     * @param name the required name
     * @return the element
     */
    public DisplayableElement get(String name) {
        if (name.equals("wind")) {
            return wind;
        }
        if (name.equals("water")) {
            return water;
        }
        for (DisplayableElement element : elements) {
            if (element.getName().equals(name)) {
                return element;
            }
        }
        return null;
    }

    /**
     * Advance time.
     *
     * @param time the new time
     */
    public void timerAdvance(int time) {
        water.timerAdvance(time);
        wind.timerAdvance(time);
        elements.stream().forEach((element) -> {
            element.timerAdvance(time);
        });
    }

    /**
     * Draw the simulation on the display canvas.
     *
     * @param g2D the 2D graphics object
     */
    public void executeAllDraw(Graphics2D g2D) {
        Double pixelsPerMetre = scenario.draw(g2D);
        water.draw(g2D, pixelsPerMetre);
        wind.draw(g2D, pixelsPerMetre);
        elements.stream().forEach((element) -> {
            element.draw(g2D, pixelsPerMetre);
        });
    }

    // add an element - used to create the executable model at time zero
    /**
     * Add a new element
     *
     * @param name the name
     * @param parameters the component definition model
     */
    public void addElement(String name, ElementParameters parameters) {
        commands.put(name, parameters);
    }

    // the key definitions data model - used to change parameters in the executable data model
    /**
     * Add a keystoke definition - this is a mapping of a keystroke to a
     * component model
     *
     * @param keystring the keystroke
     * @param parameters the component definition model
     */
    public void addKey(String keystring, ElementParameters parameters) {
        keyDefinitions.put(keystring, parameters);
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
        ElementParameters parameters = keyDefinitions.get(key);
        if (parameters != null) {
            StringBuffer errs = new StringBuffer();
            setParameters(parameters, errs);
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
     * @param parameters the revised component definition model
     */
    public void addFutureDefinition(int time, ElementParameters parameters) {
        futureDefinitions.put(time, parameters);
    }

    private void checkAllFutureDefs(StringBuffer errors) {
        futureDefinitions.keySet().stream().forEach((time) -> {
            checkParameters(futureDefinitions.get(time), errors);
        });
    }

    /**
     * Process any future updates of parameter values which have to be made at
     * this particular time.
     *
     * @param time the time
     */
    public void processDefinitions(int time) {
        ElementParameters def = futureDefinitions.remove(time);
        if (def != null) {
            StringBuffer errs = new StringBuffer();
            setParameters(def, errs);
            if (errs.length() > 0) {
                throw new LogicException("Error when processing definition at time="
                        + ScenarioSimulationDisplay.mmssformat(time) + "\n" + errs);
            }
        }
    }
}
