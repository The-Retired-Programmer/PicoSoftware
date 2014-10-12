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
package uk.org.rlinsdale.racetrainingdemonstrator.laser2;

import uk.org.rlinsdale.racetrainingdemonstrator.core.AllElements;
import uk.org.rlinsdale.racetrainingdemonstrator.boat.Boat;

/**
 * The Laser 2 Dinghy.
 * 
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public class Laser2 extends Boat {

    /**
     *Constructor
     * 
     * @param name the name 
     * @param dfm the definition file data model
     */
    public Laser2(String name, AllElements dfm) {
        super(name, dfm);
        super.metrics(4, 1.5, 0.25, 30, 45, 135); // length, width, inertia, maxturninganglepersec, upwindangle, downwindangle 
        super.performance(
                /* boat speed data */
                new double[][]{
                    //0, 40, 45, 60, 75, 90, 110, 120, 150, 180, 190
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 0
                    {-0.3, 0, 1.1, 1.1, 1.5, 2, 2, 2, 1.2, 0.8, 0.8}, // 2
                    {-0.8, 0, 2.5, 2.9, 2.9, 3.8, 3.5, 3, 2.3, 1.8, 1.8}, // 4
                    {-1.0, 0, 3.1, 3.1, 4.1, 5.6, 5, 4.2, 3.2, 2.5, 2.5}, // 6
                    {-1.1, 0, 3.5, 3.5, 4.6, 6.9, 6.5, 5.8, 4.2, 3.6, 3.6}, // 8
                    {-1.2, 0, 3.9, 3.9, 5.1, 10.9, 10.0, 8, 5, 4.5, 4.5}, // 10
                    {-1.5, 0, 4.3, 4.3, 5.7, 12.4, 13, 10, 6, 5, 5}, // 12
                    {-1.5, 0, 5.1, 5.1, 6.5, 13.5, 14.5, 12, 7.5, 5.5, 5.5}, // 14
                    {-1.5, 0, 5.2, 5.2, 7.0, 14, 17, 14, 9, 7.1, 7.1}, // 16
                    {-1.5, 0, 5.3, 5.3, 7.1, 14.5, 18, 15.8, 10.2, 7.8, 7.8}, // 18
                    {-1.5, 0, 5.3, 5.3, 7.1, 15, 19, 17.5, 12, 9, 9}, // 20
                    {-1.5, 0, 5.3, 5.3, 7.1, 15, 19, 17.5, 12, 9, 9} // 100 (backstop)
                },
                /* angular reference points used in boat speed matrix */
                new int[]{0, 40, 45, 60, 75, 90, 110, 120, 150, 180, 190},
                /* wind speed reference points used in boat speed matrix */
                new double[]{0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 100});
    }
}
