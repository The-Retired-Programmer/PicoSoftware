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

import java.util.LinkedHashMap;

/**
 * The model containing the data elements associated with a component element of
 * the definition file (eg a flow)
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class DefFileComponentModel {

    private final String instancename;
    private final String classname;
    private final LinkedHashMap<String, String> keyvaluepairs = new LinkedHashMap<>();

    /**
     * Constructor.
     *
     * @param instancename the name
     * @param classname the class name
     */
    public DefFileComponentModel(String instancename, String classname) {
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
