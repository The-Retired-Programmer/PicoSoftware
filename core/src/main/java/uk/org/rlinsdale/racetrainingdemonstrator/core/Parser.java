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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Map;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.Boat;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.BoatFactory;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.DisplayableElement;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.FlowElement;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.FlowElementFactory;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.KeyPair;
import uk.org.rlinsdale.racetrainingdemonstrator.mark.Mark;

/**
 * Parse the definition file.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class Parser {

    private enum Token {

        KEYVALUE, FAILURE, BLANK, ADDELEMENT, EOF, TIME, ACTION
    }

    private String line;
    private int time;
    private KeyPair kp;
    private String keystring;

    /**
     * Parse the definition file and load into definition data model.
     *
     * @param in1 input stream used to read the file definitions
     * @param scenario the field of play
     * @param errors a stringbuffer to append errors messages
     * @param flows the set of flows (wind and water)
     * @param marks the set of marks
     * @param boats the set of boats
     */
    public void parse(InputStream in1, StringBuffer errors, ScenarioElement scenario, Map<String, FlowElement> flows, Map<String, Mark> marks, Map<String, Boat> boats) {
        BufferedReader in = new BufferedReader(new InputStreamReader(in1));
        //
        if (parseScenario(in, errors, scenario)) {
            parseRemainder(in, errors, scenario, flows, marks, boats);
        }
    }

    private boolean parseScenario(BufferedReader in, StringBuffer errors, ScenarioElement scenario) {
        while (true) {
            try {
                line = in.readLine();
            } catch (IOException ex) {
                errors.append("IOException when reading file (").append(ex.getMessage()).append(")\n");
                return false;
            }
            switch (parseline()) {
                case ADDELEMENT:
                    return true;
                case ACTION:
                    errors.append("! command illegal within scenario context (").append(line).append(")\n");
                    break;
                case KEYVALUE:
                    scenario.setParameter(kp, errors);
                    break;
                case BLANK:
                    break; // ignore comment and blank lines
                case EOF:
                    errors.append("EOF illegal within scenario context (").append(line).append(")\n");
                    return false;
                case TIME:
                    errors.append("@ command illegal within scenario context (").append(line).append(")\n");
                    break;
                default:
                    errors.append("Illegal content (").append(line).append(")\n");
            }
        }
    }

    private void parseRemainder(BufferedReader in, StringBuffer errors, ScenarioElement scenario, Map<String, FlowElement> flows, Map<String, Mark> marks, Map<String, Boat> boats) {
        int currenttime = 0;
        DisplayableElement currentElement = getElement(kp.key, kp.value, scenario, flows, marks, boats);

        while (true) {
            try {
                line = in.readLine();
            } catch (IOException ex) {
                errors.append("IOException when reading file (").append(ex.getMessage()).append(")\n");
                return;
            }
            switch (parseline()) {
                case ADDELEMENT:
                    if (currenttime == 0) {
                        currentElement = getElement(kp.key, kp.value, scenario, flows, marks, boats);
                    } else {
                        errors.append("Can't create instances at time > 0 (").append(line).append(")\n");
                    }
                    break;
                case ACTION:
                    if (currentElement != null) {
                        currentElement.defineKeyaction(keystring, kp, errors);
                    }
                    break;
                case KEYVALUE:
                    if (currenttime == 0) {
                        if (currentElement != null) {
                            currentElement.setParameter(kp, errors);
                        }
                    } else {
                        DisplayableElement instance = extractInstanceFromKey(errors, flows, marks, boats);
                        if (instance != null) {
                            instance.setFutureParameter(currenttime, kp, errors);
                        }
                    }
                    break;
                case BLANK:
                    break; // ignore comment and blank lines
                case EOF:
                    return;
                case TIME:
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

    private DisplayableElement getElement(String instancename, String classrefname, ScenarioElement scenario, Map<String, FlowElement> flows, Map<String, Mark> marks, Map<String, Boat> boats) {
        if (instancename.equals("water") || instancename.equals("wind")) {
                FlowElement flow = FlowElementFactory.createInstance(classrefname, instancename, scenario);
                if (flow != null) {
                    flows.put(instancename, flow);
                }
            return flow;
        }
        if (classrefname.equals("mark")) {
            Mark mark = new Mark(instancename, flows.get("wind"));
                marks.put(instancename, mark);
            return mark;
        }
        Boat boat = BoatFactory.createInstance(classrefname, instancename, scenario, flows.get("wind"), flows.get("water"), marks);
            if (boat != null) {
                boats.put(instancename, boat);
            }
        return boat;
    }

    private Token parseline() {
        if (line == null) {
            return Token.EOF;
        }
        // remove comments
        int hashAt = line.indexOf('#');
        if (hashAt != -1) {
            line = line.substring(0, hashAt);
        }
        line = line.trim();
        if (line.equals("")) {
            return Token.BLANK;
        }
        if (line.startsWith("+")) {
            return parseKeyValue(1, Token.ADDELEMENT);
        }
        if (line.startsWith("!")) {
            return parseAction(1);
        }
        if (line.startsWith("@")) {
            int colonAt = line.indexOf(':');
            if (colonAt == -1) {
                try {
                    time = Integer.parseInt(line.substring(1));
                    return Token.TIME;
                } catch (NumberFormatException numberFormatException) {
                    return Token.FAILURE;
                }
            } else {
                try {
                    int minutes = Integer.parseInt(line.substring(1, colonAt));
                    int secs = Integer.parseInt(line.substring(colonAt + 1));
                    time = minutes * 60 + secs;
                    return Token.TIME;
                } catch (NumberFormatException numberFormatException) {
                    return Token.FAILURE;
                }
            }
        } else {
            return parseKeyValue(0, Token.KEYVALUE);
        }
    }

    private Token parseKeyValue(int startoffset, Token success) {
        int equalsAt = line.indexOf('=');
        if (equalsAt == -1) {
            return Token.FAILURE;
        } else {
            kp = new KeyPair(
                    line.substring(startoffset, equalsAt).replace(" ", "").toLowerCase().trim(),
                    line.substring(equalsAt + 1).replace(" ", "").toLowerCase().trim()
            );
            return success;
        }
    }

    private Token parseAction(int startoffset) {
        int colonAt = line.indexOf(':');
        if (colonAt == -1) {
            return Token.FAILURE;
        }
        keystring = line.substring(startoffset, colonAt).trim();
        return parseKeyValue(colonAt + 1, Token.ACTION);
    }

    private DisplayableElement extractInstanceFromKey(StringBuffer errors, Map<String, FlowElement> flows, Map<String, Mark> marks, Map<String, Boat> boats) {
        String fullkey = kp.key;
        //
        int dotAt = fullkey.indexOf('.');
        if (dotAt == -1) {
            errors.append("Parameter key is not dotted(").append(fullkey).append("=").append(kp.value).append(")\n");
            return null;
        }
        String instancename = fullkey.substring(0, dotAt).trim();
        kp = new KeyPair(
                fullkey.substring(dotAt + 1).trim(),
                kp.value);
        switch (instancename) {
            case "wind":
                return flows.get("wind");
            case "water":
                return flows.get("wind");
            default:
                DisplayableElement de = boats.get(instancename);
                if (de != null) {
                    return de;
                }
                de = marks.get(instancename);
                if (de != null) {
                    return de;
                }
                errors.append("No instance of name \"").append(instancename).append("\" found (").append(line).append(")\n");
                return null;
        }
    }
}
