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
package uk.theretiredprogrammer.racetrainingdemonstrator.core;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.io.FileNotFoundException;
import java.util.HashMap;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.logging.Level;
import javax.swing.Action;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JToolBar;
import javax.swing.Scrollable;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.Polar;
import uk.theretiredprogrammer.racetrainingdemonstrator.definitionfiletype.DefFileDataObject;
import uk.theretiredprogrammer.racetrainingdemonstrator.mark.Mark;
import org.netbeans.core.spi.multiview.CloseOperationState;
import org.netbeans.core.spi.multiview.MultiViewElement;
import org.netbeans.core.spi.multiview.MultiViewElementCallback;
import org.openide.awt.UndoRedo;
import org.openide.filesystems.FileChangeAdapter;
import org.openide.filesystems.FileEvent;
import org.openide.util.Lookup;
import org.openide.util.NbBundle.Messages;
import org.openide.util.lookup.Lookups;
import org.openide.util.lookup.ProxyLookup;
import org.openide.windows.TopComponent;
import uk.theretiredprogrammer.nbpcglibrary.common.LogBuilder;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.Boat;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.FlowElement;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.FlowElementFactory;
import uk.theretiredprogrammer.racetrainingdemonstrator.core.api.KeyPair;

/**
 * The Multiview DisplayableElement to display the simulation, based on the
 * definition file.
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
@MultiViewElement.Registration(
        displayName = "#LBL_DefFile_SIMULATION",
        iconBase = "uk/theretiredprogrammer/racetrainingdemonstrator/definitionfiletype/shape_flip_horizontal.png",
        mimeType = "text/x-rtd",
        persistenceType = TopComponent.PERSISTENCE_NEVER,
        preferredID = "DefFileSimulation",
        position = 2000)
@Messages("LBL_DefFile_SIMULATION=Simulation")
public final class ScenarioSimulationDisplay extends JPanel implements MultiViewElement {

    private final DefFileDataObject dataobj;
    private final JToolBar toolbar = new JToolBar();
    private final JLabel timeinfo = new JLabel("Time: 0:00");
    private JLabel firstleginfo = null;
    private transient MultiViewElementCallback callback;
    //
    private final Map<String, Boat> boats = new HashMap<>();
    private final Map<String, Mark> marks = new HashMap<>();
    private final ScenarioElement scenario = new ScenarioElement();
    private final Map<String, FlowElement> flows = new HashMap<>();
    private DisplayPanel dp;
    private boolean isRunning = false;
    private Timer timer;
    private TimeStepsRunner runner;
    private int simulationtime = 0;

    /**
     * Get the simulation display instance which is in focus.
     *
     * @return this instance
     */
    public static ScenarioSimulationDisplay getSimulationInFocus() {
        TopComponent tc = TopComponent.getRegistry().getActivated();
        if (tc == null) {
            return null;
        }
        return tc.getLookup().lookup(ScenarioSimulationDisplay.class);
    }

    /**
     * Constructor.
     *
     * @param lkp the top component lookup
     */
    public ScenarioSimulationDisplay(Lookup lkp) {
        dataobj = lkp.lookup(DefFileDataObject.class);
        assert dataobj != null;
        initComponents();
        dataobj.getPrimaryFile().addFileChangeListener(new FileChangeAdapter() {
            @Override
            public void fileChanged(FileEvent fe) {
                LogBuilder.writeLog("racetrainingdemonstrator", this, "fileChanged");
                reset();
            }
        });
        parseAndCreateSimulationDisplay();
        //
        toolbar.addSeparator();
        toolbar.add(new ResetAction(this));
        toolbar.add(new StartAction(this));
        toolbar.add(new PauseAction(this));
        toolbar.addSeparator();
        if (firstleginfo != null) {
            toolbar.add(firstleginfo);
            toolbar.addSeparator();
        }
        toolbar.add(timeinfo);
        toolbar.addSeparator();
    }

    /**
     * Start running the simulation.
     */
    public void start() {
        if (isRunning) {
            return;
        }
        LogBuilder.writeLog("racetrainingdemonstrator", this, "start");
        int rate = (int) (scenario.getSecondsPerDisplay() * 1000 / scenario.getSpeedup());
        timer = new Timer();
        runner = new TimeStepsRunner();
        timer.scheduleAtFixedRate(runner, 0, rate);
        isRunning = true;
    }

    /**
     * Reset the simulation.
     */
    public void reset() {
        terminate();
        LogBuilder.writeLog("racetrainingdemonstrator", this, "reset");
        removeAll();
        boats.values().stream().forEach((boat) -> {
            boat.finish();
        });
        marks.values().stream().forEach((mark) -> {
            mark.finish();
        });
        flows.values().stream().forEach((flow) -> {
            flow.finish();
        });
        parseAndCreateSimulationDisplay();
    }

    /**
     * Terminate the simulation.
     */
    public void terminate() {
        if (!isRunning) {
            return;
        }
        LogBuilder.writeLog("racetrainingdemonstrator", this, "terminate");
        isRunning = false;
        timer.cancel();
    }

    /**
     * Act on a function keystroke.
     *
     * @param key the keystroke
     */
    public void keyAction(String key) {
        boats.values().stream().forEach((boat) -> {
            boat.actionKey(key);
        });
        marks.values().stream().forEach((mark) -> {
            mark.actionKey(key);
        });
        flows.values().stream().forEach((flow) -> {
            flow.actionKey(key);
        });
    }

    private void parseAndCreateSimulationDisplay() {
        StringBuffer errors = new StringBuffer();
        simulationtime = 0;
        // insert default flows
        FlowElement water = FlowElementFactory.createInstance("constantflow", "water", scenario);
        flows.put("water", water);
        FlowElement wind = FlowElementFactory.createInstance("constantflow", "wind", scenario);
        wind.setParameter(new KeyPair("speed", "10"), errors);
        flows.put("wind", wind);
        try {
            Parser parser = new Parser();
            parser.parse(dataobj.getPrimaryFile().getInputStream(), errors, scenario, flows, marks, boats);
        } catch (FileNotFoundException ex) {
            errors.append("Could not open Definition file to read\n");
        }
        if (errors.length() == 0) {
            Mark m1 = marks.get(scenario.getFirstMark());
            if (m1 != null) {
                Polar p1 = new Polar(m1.getLocation(), scenario.getStartLocation());
                firstleginfo = new JLabel("First Mark: " + Integer.toString((int) p1.getDistance()) + "m");
            }
            dp = new DisplayPanel();
            this.attachPanelScrolling(dp);
        } else {
            dp = null;
            errors.insert(0, "SYNTAX ERROR in Definition File , the following errors were found\n\n");
            JPanel panel = new JPanel();
            JTextArea text = new JTextArea();
            text.setRows(200);
            text.setColumns(200);
            text.setText(errors.toString());
            text.setBackground(Color.white);
            text.setForeground(Color.red);
            panel.add(text);
            attachPanelScrolling(panel);
        }
        validate();
        repaint();
    }

    /**
     * Attach a panel to this element (embedded in a scroll pane). This would
     * typically be the display canvas for the simulation display.
     *
     * @param panel the display canvas
     */
    public void attachPanelScrolling(JPanel panel) {
        this.add(new JScrollPane(panel));
    }

    @Override
    public String getName() {
        return "DefFileSimulation";
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        setName("SimulationPanel"); // NOI18N
        setLayout(new java.awt.BorderLayout());
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables

    @Override
    public JComponent getVisualRepresentation() {
        return this;
    }

    @Override
    public JComponent getToolbarRepresentation() {
        return toolbar;
    }

    @Override
    public Action[] getActions() {
        return new Action[0];
    }

    @Override
    public Lookup getLookup() {
        return new ProxyLookup(dataobj.getLookup(), Lookups.singleton(this));
    }

    @Override
    public void componentOpened() {
    }

    @Override
    public void componentClosed() {
        terminate();
    }

    @Override
    public void componentShowing() {
    }

    @Override
    public void componentHidden() {
    }

    @Override
    public void componentActivated() {
    }

    @Override
    public void componentDeactivated() {
    }

    @Override
    public UndoRedo getUndoRedo() {
        return UndoRedo.NONE;
    }

    @Override
    public void setMultiViewCallback(MultiViewElementCallback callback) {
        this.callback = callback;
    }

    @Override
    public CloseOperationState canCloseElement() {
        return CloseOperationState.STATE_OK;
    }

    private class TimeStepsRunner extends TimerTask {

        @Override
        public void run() {
            try {
                int secondsperdisplay = scenario.getSecondsPerDisplay();
                while (secondsperdisplay > 0) {
                    boats.values().stream().forEach((boat) -> {
                        boat.actionFutureParameters(simulationtime);
                        boat.timerAdvance(simulationtime);
                    });
                    marks.values().stream().forEach((mark) -> {
                        mark.actionFutureParameters(simulationtime);
                        mark.timerAdvance(simulationtime);
                    });
                    flows.values().stream().forEach((flow) -> {
                        flow.actionFutureParameters(simulationtime);
                        flow.timerAdvance(simulationtime);
                    });
                    secondsperdisplay--;
                    simulationtime++;
                }
                timeinfo.setText("Time: " + mmssformat(simulationtime));
                dp.updateDisplay();
            } catch (Exception ex) {
                LogBuilder.create("racetrainingdemonstrator", Level.SEVERE).addExceptionMessage(ex);
            }
        }
    }

    /**
     * Format a time in seconds into mm:ss format.
     *
     * @param seconds the time interval in seconds
     * @return the time interval expressed as a mm:ss string
     */
    public static String mmssformat(int seconds) {
        int mins = seconds / 60;
        int secs = seconds % 60;
        String ss = Integer.toString(secs);
        if (ss.length() == 1) {
            ss = "0" + ss;
        }
        return Integer.toString(mins) + ":" + ss;
    }

    /**
     * The display canvas for the simulation.
     *
     * @author Richard Linsdale (richard at theretiredprogrammer.uk)
     */
    private final class DisplayPanel extends JPanel implements Scrollable {

        private final Dimension preferredsize;

        /**
         * Constructor
         *
         * @param scenario the field of play
         * @param all the list of all displayable elements
         */
        public DisplayPanel() {
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
            Double pixelsPerMetre = scenario.draw(g2D);
            boats.values().stream().forEach((boat) -> {
                boat.draw(g2D, pixelsPerMetre);
            });
            marks.values().stream().forEach((mark) -> {
                mark.draw(g2D, pixelsPerMetre);
            });
            flows.values().stream().forEach((flow) -> {
                flow.draw(g2D, pixelsPerMetre);
            });
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

}
