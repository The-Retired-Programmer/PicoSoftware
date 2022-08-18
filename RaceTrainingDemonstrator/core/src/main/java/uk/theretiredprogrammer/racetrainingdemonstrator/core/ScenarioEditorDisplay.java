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

import org.netbeans.core.spi.multiview.text.MultiViewEditorElement;
import org.openide.util.Lookup;

/**
 * The Multiview Element to display the text form of the definition file and
 * allowing it's editing.
 *
 * @author Richard Linsdale (richard at theretiredprogrammer.uk)
 */
public class ScenarioEditorDisplay extends MultiViewEditorElement {

    /**
     * Creator
     *
     * @param lkp the top component lookup
     */
    public ScenarioEditorDisplay(Lookup lkp) {
        super(lkp);
    }
}
