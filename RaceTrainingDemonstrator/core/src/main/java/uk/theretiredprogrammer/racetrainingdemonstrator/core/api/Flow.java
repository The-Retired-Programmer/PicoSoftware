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

/**
 * A movement vector containing both direction and speed
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
public class Flow {
    // direction is compass direction in degrees (-179 to +180)
    // of the flow (from)
    // speed is flow speed in knots

    private Direction direction;
    private double speed = 0; // in knots
    private static final double KNOTSTOMETRESPERSECOND = (double) 1853 / 3600; // multiply knots to get m/s
    // source NASA - 1 knot = 1.853 km/hour

    /**
     * Constructor.
     */
    public Flow() {
        direction = new Direction();
        speed = 0;
    }

    /**
     * Constructor.
     *
     * @param angle direction (degrees)
     * @param speed speed (knots)
     */
    public Flow(int angle, double speed) {
        this.direction = new Direction(angle);
        this.speed = speed;
    }

    /**
     * Constructor.
     *
     * @param copy flow - used to clone
     */
    public Flow(Flow copy) {
        this.direction = new Direction(copy.getDirection());
        this.speed = copy.getSpeedKnots();
    }

    /**
     * Normalise the direction (range is -179 to 180 degrees).
     *
     * @param direction direction (degrees)
     * @return the normalised direction (degrees)
     */
    public static int normaliseDirection(int direction) {
        while (direction > 180) {
            direction -= 360;
        }
        while (direction <= -180) {
            direction += 360;
        }
        return direction;
    }

    /**
     * Set the direction of a flow.
     *
     * @param angle direction (degrees)
     */
    public void setAngle(int angle) {
        this.direction.set(angle);
    }

    /**
     * Get the direction of a flow.
     *
     * @return direction (degrees)
     */
    public int getAngle() {
        return direction.getDegrees();
    }

    /**
     * Get the direction of a flow.
     *
     * @return direction
     */
    public Direction getDirection() {
        return direction;
    }

    /**
     * Set the direction of a flow.
     *
     * @param direction direction
     */
    public void setDirection(Direction direction) {
        this.direction = direction;
    }

    /**
     * Set the speed of a flow.
     *
     * @param speed speed (knots)
     */
    public void setSpeedKnots(double speed) {
        this.speed = speed;
    }

    /**
     * Get the speed of a flow.
     *
     * @return speed (knots)
     */
    public double getSpeedKnots() {
        return speed;
    }

    /**
     * Get the speed of a flow.
     *
     * @return speed (metres/second)
     */
    public double getSpeedMetresPerSecond() {
        return speed * KNOTSTOMETRESPERSECOND;
    }

    /**
     * Convert Knots to Metres/Second
     *
     * @param knots speed
     * @return speed (metres/sec)
     */
    public static double convertKnots2MetresPerSecond(double knots) {
        return knots * KNOTSTOMETRESPERSECOND;
    }
}
