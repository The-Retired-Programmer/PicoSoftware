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
import linsdale.nbpcg.supportlib.Log;
import linsdale.nbpcg.supportlib.OutputReporter;
import linsdale.rtd.core.api.Polar;
import linsdale.rtd.definitionfiletype.DefFileDataObject;
import linsdale.rtd.mark.Mark;
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
 *
 * @author Richard Linsdale (richard.linsdale at blueyonder.co.uk)
 */
@MultiViewElement.Registration(
        displayName = "#LBL_DefFile_SIMULATION",
        iconBase = "linsdale/rtd/definitionfiletype/shape_flip_horizontal.png",
        mimeType = "text/x-rta",
        persistenceType = TopComponent.PERSISTENCE_NEVER,
        preferredID = "DefFileSimulation",
        position = 2000)
@Messages("LBL_DefFile_SIMULATION=Simulation")
public final class ScenarioSimulationElement extends JPanel implements MultiViewElement {

    private final DefFileDataObject dataobj;
    private final JToolBar toolbar = new JToolBar();
    private final JLabel timeinfo = new JLabel("Time: 0:00");
    private JLabel firstleginfo = null;
    private transient MultiViewElementCallback callback;
    //
    private DefFileModel dfm;
    private DisplayPanel dp;
    private boolean isRunning = false;
    private Timer timer;
    private TimeStepsRunner runner;
    private int simulationtime = 0;
    private OutputReporter reporter;

    public static ScenarioSimulationElement getSimulationInFocus() {
        TopComponent tc = TopComponent.getRegistry().getActivated();
        if (tc == null) {
            return null;
        }
        return tc.getLookup().lookup(ScenarioSimulationElement.class);
    }

    public ScenarioSimulationElement(Lookup lkp) {
        dataobj = lkp.lookup(DefFileDataObject.class);
        assert dataobj != null;
        initComponents();
        dataobj.getPrimaryFile().addFileChangeListener(new FileChangeAdapter() {
            @Override
            public void fileChanged(FileEvent fe) {
                Log.get("linsdale.rta").finer("File change detected");
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

    public void start() {
        if (isRunning) {
            return;
        }
        Scenario scenario = dfm.getScenario();
        Log.get("linsdale.rta").finer("Simulation started");
        int rate = (int) (scenario.getSecondsPerDisplay() * 1000 / scenario.getSpeedup());
        timer = new Timer();
        runner = new TimeStepsRunner();
        timer.scheduleAtFixedRate(runner, 0, rate);
        isRunning = true;
    }

    public void reset() {
        terminate();
        Log.get("linsdale.rta").finer("Simulation reset");
        removeAll();
        dfm.finish();
        parseAndCreateSimulationDisplay();
    }

    public void terminate() {
        if (!isRunning) {
            return;
        }
        Log.get("linsdale.rta").finer("Simulation terminated");
        isRunning = false;
        timer.cancel();
    }

    public void keyAction(String key) {
        dfm.processKey(key);
    }

    private void parseAndCreateSimulationDisplay() {
        StringBuffer errors = new StringBuffer();
        dfm = new DefFileModel();
        simulationtime = 0;
        try {
            dfm.load(reporter, dataobj.getPrimaryFile().getInputStream(), errors);
        } catch (FileNotFoundException ex) {
            errors.append("Could not open Definition file to read\n");
        }
        if (errors.length() == 0) {
            Scenario sc = dfm.getScenario();
            Mark m1 = (Mark) dfm.getNamedInstance(sc.getFirstMark());
            if (m1 != null) {
                Polar p1 = new Polar(m1.getLocation(), sc.getStartLocation());

                firstleginfo = new JLabel("First Mark: " + Integer.toString((int) p1.getDistance()) + "m");
            }
            dp = new DisplayPanel(dfm);
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
        reporter = new OutputReporter(dataobj.getPrimaryFile().getNameExt(), "");
    }

    @Override
    public void componentClosed() {
        terminate();
        dfm.finish();
        reporter.close();
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
                Scenario scenario = dfm.getScenario();
                int secondsperdisplay = scenario.getSecondsPerDisplay();
                while (secondsperdisplay > 0) {
                    dfm.processDefinitions(simulationtime);
                    dfm.executeAllTimeAdvance(simulationtime);
                    secondsperdisplay--;
                    simulationtime++;
                }
                timeinfo.setText("Time: " + mmssformat(simulationtime));
                dp.updateDisplay();
            } catch (Exception ex) {
                Log.get("linsdale.rta").log(Level.SEVERE, null, ex);
            }
        }
    }

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
