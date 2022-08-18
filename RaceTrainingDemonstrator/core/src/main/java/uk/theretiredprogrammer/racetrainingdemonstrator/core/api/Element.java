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
package uk.theretiredprogrammer.racetrainingdemonstrator.core.api;

import java.awt.Color;
import java.util.List;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status;

/**
 * The Abstract Class - represents the core class of every simulation object
 * class.
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
abstract public class Element {

    final protected String name;

    /**
     * Parameter parsed/set OK
     * @param name the element name
     */
    public Element(String name) {
        this.name = name;
    }

    /**
     * Parse a yes/no parameter value
     *
     * @param value the string to parse
     * @return the boolean result
     * @throws NumberFormatException if not yes or no
     */
    protected boolean parseYesNo(String value) throws NumberFormatException {
        if (value.equals("yes")) {
            return true;
        }
        if (value.equals("no")) {
            return false;
        }
        throw new NumberFormatException("Illegal yes/no boolean");
    }

    /**
     * Parse a location parameter
     *
     * @param value the location string (xxx,yyy) parameter
     * @return a location
     * @throws NumberFormatException if not a location
     */
    protected Location parseLocation(String value) throws NumberFormatException {
        String[] coords = value.split(",");
        int l = coords.length;
        if (l != 2) {
            throw new NumberFormatException("Location should be defined as two numbers");
        }
        Location loc = new Location();
        loc.x = Double.parseDouble(coords[0]);
        loc.y = Double.parseDouble(coords[1]);
        return loc;
    }

    /**
     * Parse a colour parameter
     *
     * @param value a colour string
     * @return a Color
     * @throws NumberFormatException if not a color
     */
    protected Color parseColour(String value) throws NumberFormatException {
        switch (value) {
            case "black":
                return Color.BLACK;
            case "blue":
                return Color.BLUE;
            case "cyan":
                return Color.cyan;
            case "darkgrey":
                return Color.DARK_GRAY;
            case "grey":
                return Color.GRAY;
            case "green":
                return Color.GREEN;
            case "lightgrey":
                return Color.LIGHT_GRAY;
            case "magenta":
                return Color.MAGENTA;
            case "orange":
                return Color.ORANGE;
            case "pink":
                return Color.PINK;
            case "red":
                return Color.RED;
            case "white":
                return Color.WHITE;
            case "yellow":
                return Color.YELLOW;
            default:
                String[] rgba = value.split(",");
                int l = rgba.length;
                if (l < 3 || l > 4) {
                    throw new NumberFormatException("Too many or too few colour components");
                }
                int[] rgbaValues = new int[4];
                for (int i = 0; i < l; i++) {
                    rgbaValues[i] = Integer.parseInt(rgba[i].trim());
                }
                if (l == 3) {
                    rgbaValues[3] = 255;
                }
                try {
                    return new Color(rgbaValues[0], rgbaValues[1],
                            rgbaValues[2], rgbaValues[3]);
                } catch (IllegalArgumentException e) {
                    throw new NumberFormatException(e.getMessage());
                }
        }
    }

    /**
     * Get the entity name
     *
     * @return the name
     */
    public String getName() {
        return name;
    }

    /**
     * Set a number of parameters.
     *
     * @param kps list of key pairs
     * @param errors a StringBuffer to collect error messages
     */
    public void setParameters(List<KeyPair> kps, StringBuffer errors) {
        kps.stream().forEach((kp) -> {
            setParameter(kp, errors);
        });
    }

    public void setParameter(KeyPair kp, StringBuffer errors) {
        switch (setParameter(kp)) {
            case OK:
                break;
            case BADKEY:
                errors.append("Attempting to set undefined key (").append(name).append(".").append(kp.key).append("=").append(kp.value).append(")\n");
                break;
            case BADVALUE:
                errors.append("Attempting to set key with bad value (").append(name).append(".").append(kp.key).append("=").append(kp.value).append(")\n");
                break;
            default:
                errors.append("Bad return code - SYSTEM PROBLEM!\n");
                break;
        }
    }

    /**
     * Set a parameter.
     *
     * @param kp the key/value pair
     * @return success code
     */
    abstract protected Status setParameter(KeyPair kp);

}
