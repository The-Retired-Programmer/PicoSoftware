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

/**
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class Direction {

    public double direction = 0;

    public Direction() {
        direction = 0;
    }

    public Direction(Direction d) {
        direction = d.getRadians();
    }

    public Direction(Direction d, int degrees) {
        direction = d.getRadians();
        add(degrees);
    }

    public Direction(int degrees) {
        set(degrees);
    }

    public Direction(Direction d, double radians) {
        direction = d.getRadians();
        add(radians);
    }

    public Direction(double radians) {
        set(radians);
    }

    public Direction(Location from, Location to) {
        direction = Math.atan2(to.x - from.x, to.y - from.y);
    }

    public final void set(Direction d) {
        direction = d.direction;
        normaliseDirection();
    }

    public final void set(int degrees) {
        direction = Math.toRadians(degrees);
        normaliseDirection();
    }

    public final int getDegrees() {
        return (int) Math.round(Math.toDegrees(direction));
    }

    public final void add(int degrees) {
        direction += Math.toRadians(degrees);
        normaliseDirection();
    }

    public final void subtract(int degrees) {
        direction -= Math.toRadians(degrees);
        normaliseDirection();
    }

    public final int angleDiff(int degrees) {
        return (int) Math.toDegrees(normalise(Math.toRadians(degrees) - direction));
    }

    public final int angleDiff(Direction d) {
        return angleDiff(d.getDegrees());
    }

    public final int absAngleDiff(int degrees) {
        return Math.abs(angleDiff(degrees));
    }

    public final int absAngleDiff(Direction d) {
        return Math.abs(angleDiff(d));
    }

    public final void set(double radians) {
        direction = radians;
        normaliseDirection();
    }

    public final double getRadians() {
        return direction;
    }

    public final void add(double radians) {
        direction += radians;
        normaliseDirection();
    }

    public final void subtract(double radians) {
        direction -= radians;
        normaliseDirection();
    }

    public final double sin() {
        return Math.sin(direction);
    }

    public final double cos() {
        return Math.cos(direction);
    }

    public final double tan() {
        return Math.tan(direction);
    }

    public static double normalise(double direction) {
        while (direction > Math.PI) {
            direction -= 2 * Math.PI;
        }
        while (direction < -Math.PI) {
            direction += 2 * Math.PI;
        }
        return direction;
    }

    private void normaliseDirection() {
        while (direction > Math.PI) {
            direction -= 2 * Math.PI;
        }
        while (direction < -Math.PI) {
            direction += 2 * Math.PI;
        }
    }
}
