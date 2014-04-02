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
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class DefFileComponentModel {

    private final String instancename;
    private final String classname;
    private final LinkedHashMap<String, String> keyvaluepairs = new LinkedHashMap<>();

    public DefFileComponentModel(String instancename, String classname) {
        this.instancename = instancename;
        this.classname = classname;
    }

    public void addparameter(String key, String value) {
        keyvaluepairs.put(key, value);
    }

    public LinkedHashMap<String, String> getParameters() {
        return keyvaluepairs;
    }

    public String getClassname() {
        return classname;
    }

    public String getInstancename() {
        return instancename;
    }
}
