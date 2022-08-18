/*
 * Copyright 2014-2018 Richard Linsdale.
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
package uk.theretiredprogrammer.racetrainingdemonstrator.lifecycle;

import uk.theretiredprogrammer.nbpcglibrary.lifecycle.BasicLifeCycle;
import org.openide.modules.OnStart;

/**
 *  LifecycleManagement
 *
 * @author Richard Linsdale - richard at theretiredprogrammer.uk
 */
@OnStart
public class RaceTrainingDemonstratorLifeCycle extends BasicLifeCycle implements Runnable {

    /**
     * Constructor.
     */
    public RaceTrainingDemonstratorLifeCycle() {
        super(RaceTrainingDemonstratorLifeCycle.class.getResourceAsStream("/META-INF/application.properties"));
    }

    @Override
    public void run() {
    }
}
