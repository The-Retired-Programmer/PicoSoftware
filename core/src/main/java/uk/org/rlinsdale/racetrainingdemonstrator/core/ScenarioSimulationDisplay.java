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

import java.awt.Color;
import java.io.FileNotFoundException;
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
import uk.org.rlinsdale.nbpcglibrary.common.Log;
import uk.org.rlinsdale.racetrainingdemonstrator.core.api.Polar;
import uk.org.rlinsdale.racetrainingdemonstrator.definitionfiletype.DefFileDataObject;
import uk.org.rlinsdale.racetrainingdemonstrator.mark.Mark;
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

/**
 * The Multiview DisplayableElement to display the simulation, based on the definition
 file.
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
@MultiViewElement.Registration(
        displayName = "#LBL_DefFile_SIMULATION",
        iconBase = "uk/org/rlinsdale/racetrainingdemonstrator/definitionfiletype/shape_flip_horizontal.png",
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
    private AllElements allelements;
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
                Log.get("uk.org.rlinsdale.racetrainingdemonstrator").finer("File change detected");
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
        ScenarioElement scenario = allelements.getScenarioElement();
        Log.get("uk.org.rlinsdale.racetrainingdemonstrator").finer("Simulation started");
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
        Log.get("uk.org.rlinsdale.racetrainingdemonstrator").finer("Simulation reset");
        removeAll();
        allelements.finish();
        parseAndCreateSimulationDisplay();
    }

    /**
     * Terminate the simulation.
     */
    public void terminate() {
        if (!isRunning) {
            return;
        }
        Log.get("uk.org.rlinsdale.racetrainingdemonstrator").finer("Simulation terminated");
        isRunning = false;
        timer.cancel();
    }

    /**
     * Act on a function keystroke.
     *
     * @param key the keystroke
     */
    public void keyAction(String key) {
        allelements.processKey(key);
    }

    private void parseAndCreateSimulationDisplay() {
        StringBuffer errors = new StringBuffer();
        allelements = new AllElements();
        simulationtime = 0;
        try {
            allelements.load(dataobj.getPrimaryFile().getInputStream(), errors);
        } catch (FileNotFoundException ex) {
            errors.append("Could not open Definition file to read\n");
        }
        if (errors.length() == 0) {
            ScenarioElement sc = allelements.getScenarioElement();
            Mark m1 = (Mark) allelements.get(sc.getFirstMark());
            if (m1 != null) {
                Polar p1 = new Polar(m1.getLocation(), sc.getStartLocation());

                firstleginfo = new JLabel("First Mark: " + Integer.toString((int) p1.getDistance()) + "m");
            }
            dp = new DisplayPanel(allelements);
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
                ScenarioElement scenario = allelements.getScenarioElement();
                int secondsperdisplay = scenario.getSecondsPerDisplay();
                while (secondsperdisplay > 0) {
                    allelements.processDefinitions(simulationtime);
                    allelements.timerAdvance(simulationtime);
                    secondsperdisplay--;
                    simulationtime++;
                }
                timeinfo.setText("Time: " + mmssformat(simulationtime));
                dp.updateDisplay();
            } catch (Exception ex) {
                Log.get("uk.org.rlinsdale.racetrainingdemonstrator").log(Level.SEVERE, null, ex);
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
}
