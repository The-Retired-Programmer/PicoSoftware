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
package uk.theretiredprogrammer.racetrainingdemonstrator.core;

import java.util.LinkedHashMap;

/**
 * The model containing the data elements associated with a component element of
 * the definition file (eg a flow)
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
public class ElementParameters {

    private final String instancename;
    private final String classname;
    private final LinkedHashMap<String, String> keyvaluepairs = new LinkedHashMap<>();

    /**
     * Constructor.
     *
     * @param instancename the name
     * @param classname the class name
     */
    public ElementParameters(String instancename, String classname) {
        this.instancename = instancename;
        this.classname = classname;
    }

    /**
     * Add a parameter and its value.
     *
     * @param key the parameter key
     * @param value the parameter value
     */
    public void addparameter(String key, String value) {
        keyvaluepairs.put(key, value);
    }

    /**
     * Get the collection of all parameters.
     *
     * @return the parameters
     */
    public LinkedHashMap<String, String> getParameters() {
        return keyvaluepairs;
    }

    /**
     * Get the class name of this component
     *
     * @return the class name
     */
    public String getClassname() {
        return classname;
    }

    /**
     * Get the instance name of this component
     *
     * @return the instance name
     */
    public String getInstancename() {
        return instancename;
    }
}
