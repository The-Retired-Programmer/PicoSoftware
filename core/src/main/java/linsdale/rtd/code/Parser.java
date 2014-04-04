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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * Parse the definition file.
 * 
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class Parser {

    private final int PARSEKEYVALUE = 1;
    private final int PARSEFAILURE = -1;
    private final int PARSEBLANK = -2;
    private final int PARSEADDINSTANCE = 2;
    private final int PARSEEOF = 0;
    private final int PARSETIME = 3;
    private final int PARSEACTION = 4;
    private int time;
    private String instancename;
    private String key;
    private String value;
    private String keystring;

    /**
     * Parse the definition file and load into definition data model.
     * 
     * @param in1 input stream used to read the file definitions
     * @param dm the definition data model
     * @param errors a stringbuffer to append errors messages
     */
    public void parse(InputStream in1, DefFileModel dm, StringBuffer errors) {
        int currenttime = 0;
        BufferedReader in = new BufferedReader(new InputStreamReader(in1));
        DefFileComponentModel currentInstanceDefinition = new DefFileComponentModel("scenario", "scenario");
        dm.addInstanceDefinition("scenario", currentInstanceDefinition);
        while (true) {
            String line;
            try {
                line = in.readLine();
            } catch (IOException ex) {
                errors.append("IOException when reading file (").append(ex.getMessage()).append(")\n");
                return;
            }
            switch (parseline(line)) {
                case PARSEADDINSTANCE:
                    if (currenttime == 0) {
                        currentInstanceDefinition = new DefFileComponentModel(key, value);
                        dm.addInstanceDefinition(key, currentInstanceDefinition);
                        instancename = key;
                    } else {
                        errors.append("Can't create instances at time > 0 (").append(line).append(")\n");
                    }
                    break;
                case PARSEACTION:
                    DefFileComponentModel def = new DefFileComponentModel(instancename, null);
                    def.addparameter(key, value);
                    dm.addkeyDefinition(keystring, def);
                    break;
                case PARSEKEYVALUE:
                    if (currenttime == 0) {
                        currentInstanceDefinition.addparameter(key, value);
                    } else {
                        extractObjectnameFromKey(errors);
                        DefFileComponentModel fdef = new DefFileComponentModel(instancename, null);
                        dm.addFutureDefinition(currenttime, fdef);
                        fdef.addparameter(key, value);
                    }
                    break;
                case PARSEBLANK:
                    break; // ignore comment and blank lines
                case PARSEEOF:
                    return;
                case PARSETIME:
                    if (time > currenttime) {
                        currenttime = time;
                    } else {
                        errors.append("Time statement must advance time (").append(line).append(")\n");
                    }
                    break;
                default:
                    errors.append("Illegal content (").append(line).append(")\n");
            }
        }
    }

    private int parseline(String line) {
        if (line == null) {
            return PARSEEOF;
        }
        // remove comments
        int hashAt = line.indexOf('#');
        if (hashAt != -1) {
            line = line.substring(0, hashAt);
        }
        line = line.trim();
        if (line.equals("")) {
            return PARSEBLANK;
        }
        if (line.startsWith("+")) {
            return parseKeyValue(line, 1, PARSEADDINSTANCE);
        }
        if (line.startsWith("!")) {
            return parseAction(line.substring(1).trim());
        }
        if (line.startsWith("@")) {
            int colonAt = line.indexOf(':');
            if (colonAt == -1) {
                try {
                    time = Integer.parseInt(line.substring(1));
                    return PARSETIME;
                } catch (NumberFormatException numberFormatException) {
                    return PARSEFAILURE;
                }
            } else {
                try {
                    int minutes = Integer.parseInt(line.substring(1, colonAt));
                    int secs = Integer.parseInt(line.substring(colonAt + 1));
                    time = minutes * 60 + secs;
                    return PARSETIME;
                } catch (NumberFormatException numberFormatException) {
                    return PARSEFAILURE;
                }
            }
        } else {
            return parseKeyValue(line, 0, PARSEKEYVALUE);
        }
    }

    private int parseKeyValue(String line, int startoffset, int success) {
        int equalsAt = line.indexOf('=');
        if (equalsAt == -1) {
            return PARSEFAILURE;
        } else {
            key = line.substring(startoffset, equalsAt).replace(" ", "").toLowerCase().trim();
            value = line.substring(equalsAt + 1).replace(" ", "").toLowerCase().trim();
            return success;
        }
    }

    private int parseAction(String line) {
        int equalsColon = line.indexOf(':');
        if (equalsColon == -1) {
            return PARSEFAILURE;
        }
        keystring = line.substring(1, equalsColon).trim();
        return parseKeyValue(line, equalsColon + 1, PARSEACTION);
    }

    private void extractObjectnameFromKey(StringBuffer errors) {
        String fullkey = key;
        //
        int dotAt = fullkey.indexOf('.');
        if (dotAt == -1) {
            errors.append("Parameter key is not dotted(").append(fullkey).append("=").append(value).append(")\n");
        } else {
            instancename = fullkey.substring(0, dotAt).trim();
            key = fullkey.substring(dotAt + 1).trim();
        }
    }
}
