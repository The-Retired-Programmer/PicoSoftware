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
package linsdale.rtd.core.api;

import java.awt.Color;
import java.util.LinkedHashMap;

/**
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
abstract public class RTABaseObject {

    public static final int PARAM_OK = 0;
    public static final int PARAM_BADKEY = 1;
    public static final int PARAM_BADVALUE = 2;
    protected String name;

    public RTABaseObject(String name) {
        this.name = name;
    }

    protected boolean parseYesNo(String value) {
        if (value.equals("yes")) {
            return true;
        }
        if (value.equals("no")) {
            return false;
        }
        throw new NumberFormatException("Illegal yes/no boolean");
    }

    protected Location parseLocation(String value) {
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

    protected Color parseColour(String value) {
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

    public String getName() {
        return name;
    }

    public void setParameters(LinkedHashMap<String, String> parameters, StringBuffer errors) {
        for (String key : parameters.keySet()) {
            String value = parameters.get(key);
            switch (setParameter(key, value)) {
                case linsdale.rtd.core.api.RTAObject.PARAM_OK:
                    break;
                case linsdale.rtd.core.api.RTAObject.PARAM_BADKEY:
                    errors.append("Attempting to set undefined key (").append(name).append(".").append(key).append("=").append(value).append(")\n");
                    break;
                case linsdale.rtd.core.api.RTAObject.PARAM_BADVALUE:
                    errors.append("Attempting to set key with bad value (").append(name).append(".").append(key).append("=").append(value).append(")\n");
                    break;
                default:
                    errors.append("Bad return code - SYSTEM PROBLEM!\n");
                    break;
            }
        }
    }

    abstract public int setParameter(String key, String value);

    public void checkParameters(LinkedHashMap<String, String> parameters, StringBuffer errors) {
        for (String key : parameters.keySet()) {
            String value = parameters.get(key);
            switch (checkParameter(key, value)) {
                case linsdale.rtd.core.api.RTAObject.PARAM_OK:
                    break;
                case linsdale.rtd.core.api.RTAObject.PARAM_BADKEY:
                    errors.append("Attempting to set undefined key (").append(name).append(".").append(key).append("=").append(value).append(")\n");
                    break;
                case linsdale.rtd.core.api.RTAObject.PARAM_BADVALUE:
                    errors.append("Attempting to set key with bad value (").append(name).append(".").append(key).append("=").append(value).append(")\n");
                    break;
                default:
                    errors.append("Bad return code - SYSTEM PROBLEM!\n");
                    break;
            }
        }
    }

    abstract public int checkParameter(String key, String value);
}
