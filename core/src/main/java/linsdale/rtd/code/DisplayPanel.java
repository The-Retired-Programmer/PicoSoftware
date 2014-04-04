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

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import javax.swing.JPanel;
import javax.swing.Scrollable;

/**
 * The display canvas for the simulation.
 * 
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
public final class DisplayPanel extends JPanel implements Scrollable {

    private final DefFileModel dfm;
    private final Dimension preferredsize;

    /**
     * Constructor
     * 
     * @param dfm the definition file datamodel
     */
    public DisplayPanel(DefFileModel dfm) {
        this.dfm = dfm;
        Scenario scenario = dfm.getScenario();
        double width = scenario.east - scenario.west;
        double depth = scenario.north - scenario.south;
        double scale = scenario.scale;
        preferredsize = new Dimension((int) (width * scale), (int) (depth * scale));
    }

    /**
     * Update the display
     */
    public void updateDisplay() {
        this.repaint();
    }

    @Override
    public void paintComponent(Graphics g) {
        this.setBackground(new Color(200, 255, 255));
        super.paintComponent(g);
        Graphics2D g2D = (Graphics2D) g;
        // set the rendering hints
        g2D.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2D.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
        // now draw the various items
        dfm.executeAllDraw(g2D);
    }

    @Override
    public Dimension getPreferredSize() {
        return preferredsize;
    }

    @Override
    public Dimension getPreferredScrollableViewportSize() {
        return preferredsize;
    }

    @Override
    public int getScrollableUnitIncrement(Rectangle visibleRect, int orientation, int direction) {
        return 20;
    }

    @Override
    public int getScrollableBlockIncrement(Rectangle visibleRect, int orientation, int direction) {
        return 200;
    }

    @Override
    public boolean getScrollableTracksViewportWidth() {
        return false;
    }

    @Override
    public boolean getScrollableTracksViewportHeight() {
        return false;
    }
}
