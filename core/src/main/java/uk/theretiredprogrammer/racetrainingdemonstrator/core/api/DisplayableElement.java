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

import java.awt.Graphics2D;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair.Status;

/**
 * The abstract class defining the common interface for all simulation entities.
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
abstract public class DisplayableElement extends Element {
    
     private final Map<String, KeyPair>keyactions = new HashMap<>();
     private final Map<Integer, KeyPair>futureactions = new HashMap<>();
    
    public DisplayableElement(String name) {
        super(name);
    }
    
    public void defineKeyaction(String key, KeyPair kp, StringBuffer errors) {
        checkParameter(kp, errors);
        if (errors.length() == 0) {
            keyactions.put(key, kp);
        }
    }
    
    public void actionKey(String key) {
        KeyPair kp = keyactions.get(key);
        if (kp != null) {
            setParameter(kp);
        }
    }
    
    public void setFutureParameter(int time, KeyPair kp, StringBuffer errors){
        checkParameter(kp, errors);
        if (errors.length() == 0) {
            futureactions.put(time, kp);
        }
    }
    
    /**
     * Process any future updates of parameter values which have to be made at
     * this particular time.
     *
     * @param time the time
     */
    public void actionFutureParameters(int time) {
        KeyPair kp = futureactions.get(time);
        if (kp != null) {
            setParameter(kp);
        }
    }
    
    /**
     * Finish the simulation.
     */
    public void finish() {
    }

    /**
     * Advance the time to the given time.
     *
     * @param time the time
     */
    abstract public void timerAdvance(int time);

    /**
     * Draw the Flow arrows on the display canvas.
     *
     * @param g2D the 2D graphics object
     * @param pixelsPerMetre the scale factor
     */
    abstract public void draw(Graphics2D g2D, double pixelsPerMetre);
    
       /**
     * Check a number of parameters.
     *
     * @param kps list of key pairs
     * @param errors a StringBuffer to collect error messages
     */
    protected void checkParameters(List<KeyPair> kps, StringBuffer errors) {
        kps.stream().forEach((kp) -> {
            checkParameter(kp, errors);
         });
    }

    protected void checkParameter(KeyPair kp, StringBuffer errors) {
        switch (checkParameter(kp)) {
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
     * Check the legality of a particular Parameter value
     *
     * @param kp the key and value pair to be checked
     * @return success code
     */
    abstract protected Status checkParameter(KeyPair kp);
}
