package com.beegfs.admon.gui.components.internalframes.management;

import com.beegfs.admon.gui.common.XMLParser;
import com.beegfs.admon.gui.common.exceptions.CommunicationException;
import com.beegfs.admon.gui.common.tools.GuiTk;
import com.beegfs.admon.gui.common.tools.HttpTk;
import com.beegfs.admon.gui.components.internalframes.JInternalFrameInterface;
import com.beegfs.admon.gui.components.managers.FrameManager;
import com.beegfs.admon.gui.program.Main;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.Box;
import javax.swing.JLabel;


public class JInternalFrameKnownProblems extends javax.swing.JInternalFrame implements
   JInternalFrameInterface
{
   static final Logger LOGGER = Logger.getLogger(
      JInternalFrameKnownProblems.class.getCanonicalName());
   private static final long serialVersionUID = 1L;
   private static final String THREAD_NAME = "KnownProblems";

   private final transient XMLParser parser;

   public JInternalFrameKnownProblems()
   {
      initComponents();
      setTitle(getFrameTitle());
      setFrameIcon(GuiTk.getFrameIcon());
      parser = new XMLParser(HttpTk.generateAdmonUrl("/XML_KnownProblems"), THREAD_NAME);
   }
  
   @Override
   public boolean isEqual(JInternalFrameInterface obj)
   {
      return obj instanceof JInternalFrameKnownProblems;
   }

   @Override
   public final String getFrameTitle()
   {
	   return Main.getLocal().getString("Known Problems");
   }

   private void startKnownProblemsUpdate()
   {
      parser.start();
      updateData();
      int delay = 10000; //milliseconds
      ActionListener taskPerformer = new ActionListener()
      {
         @Override
         public void actionPerformed(ActionEvent evt)
         {
            updateData();
         }

      };
      new javax.swing.Timer(delay, taskPerformer).start();
   }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
   // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
   private void initComponents()
   {

      jScrollPaneFrame = new javax.swing.JScrollPane();
      jPanelFrame = new javax.swing.JPanel();

      setClosable(true);
      setIconifiable(true);
      setMaximizable(true);
      setResizable(true);
      setMinimumSize(new java.awt.Dimension(0, 0));
      setPreferredSize(new java.awt.Dimension(440, 180));
      addInternalFrameListener(new javax.swing.event.InternalFrameListener()
      {
         public void internalFrameOpened(javax.swing.event.InternalFrameEvent evt)
         {
            formInternalFrameOpened(evt);
         }
         public void internalFrameClosing(javax.swing.event.InternalFrameEvent evt)
         {
         }
         public void internalFrameClosed(javax.swing.event.InternalFrameEvent evt)
         {
            formInternalFrameClosed(evt);
         }
         public void internalFrameIconified(javax.swing.event.InternalFrameEvent evt)
         {
         }
         public void internalFrameDeiconified(javax.swing.event.InternalFrameEvent evt)
         {
         }
         public void internalFrameActivated(javax.swing.event.InternalFrameEvent evt)
         {
         }
         public void internalFrameDeactivated(javax.swing.event.InternalFrameEvent evt)
         {
         }
      });

      jScrollPaneFrame.setMinimumSize(new java.awt.Dimension(0, 0));

      jPanelFrame.setPreferredSize(new java.awt.Dimension(382, 120));
      jPanelFrame.setLayout(new javax.swing.BoxLayout(jPanelFrame, javax.swing.BoxLayout.PAGE_AXIS));
      jScrollPaneFrame.setViewportView(jPanelFrame);

      javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
      getContentPane().setLayout(layout);
      layout.setHorizontalGroup(
         layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
         .addGroup(layout.createSequentialGroup()
            .addContainerGap()
            .addComponent(jScrollPaneFrame, javax.swing.GroupLayout.DEFAULT_SIZE, 405, Short.MAX_VALUE)
            .addContainerGap())
      );
      layout.setVerticalGroup(
         layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
         .addGroup(layout.createSequentialGroup()
            .addContainerGap()
            .addComponent(jScrollPaneFrame, javax.swing.GroupLayout.DEFAULT_SIZE, 126, Short.MAX_VALUE)
            .addContainerGap())
      );

      pack();
   }// </editor-fold>//GEN-END:initComponents

    private void formInternalFrameClosed(javax.swing.event.InternalFrameEvent evt) {//GEN-FIRST:event_formInternalFrameClosed
      FrameManager.delFrame(this);
    }//GEN-LAST:event_formInternalFrameClosed

   private void formInternalFrameOpened(javax.swing.event.InternalFrameEvent evt)//GEN-FIRST:event_formInternalFrameOpened
   {//GEN-HEADEREND:event_formInternalFrameOpened
      this.startKnownProblemsUpdate();
   }//GEN-LAST:event_formInternalFrameOpened


   // Variables declaration - do not modify//GEN-BEGIN:variables
   private javax.swing.JPanel jPanelFrame;
   private javax.swing.JScrollPane jScrollPaneFrame;
   // End of variables declaration//GEN-END:variables

   private void updateData()
   {
      jPanelFrame.removeAll();
      jPanelFrame.revalidate();
      jPanelFrame.repaint();
      try
      {
         boolean problems = false;
         ArrayList<String> deadMetaNodes = parser.getVector("deadMetaNodes");
         if (!deadMetaNodes.isEmpty() )
         {
            problems = true;
            for (String node : deadMetaNodes)
            {
               JLabel label = new JLabel("Metadata node \"" + node + "\" seems to be dead.");
               label.setForeground(new Color(180, 0, 30));
               label.setAlignmentX(JLabel.CENTER_ALIGNMENT);
               label.setHorizontalAlignment(JLabel.CENTER);
               label.setHorizontalTextPosition(JLabel.CENTER);
               jPanelFrame.add(label);
               jPanelFrame.add(Box.createRigidArea(new Dimension(0, 5)));
            }
         }
         ArrayList<String> deadStorageNodes = parser.getVector("deadStorageNodes");
         if (!deadStorageNodes.isEmpty() )
         {
            problems = true;
            for (String node : deadStorageNodes)
            {
               JLabel label = new JLabel("Storage node \"" + node + "\" seems to be dead.");
               label.setForeground(GuiTk.getRed());
               label.setAlignmentX(JLabel.CENTER_ALIGNMENT);
               label.setHorizontalAlignment(JLabel.CENTER);
               label.setHorizontalTextPosition(JLabel.CENTER);
               jPanelFrame.add(label);
               jPanelFrame.add(Box.createRigidArea(new Dimension(0, 5)));
            }
         }

         if (!problems)
         {
        	 JLabel label = new JLabel(Main.getLocal().getString("No known problems."));
            label.setAlignmentX(JLabel.CENTER_ALIGNMENT);
            label.setHorizontalAlignment(JLabel.CENTER);
            label.setHorizontalTextPosition(JLabel.CENTER);
            label.setForeground(GuiTk.getGreen());
            jPanelFrame.add(label);
         }
         jPanelFrame.revalidate();
         jPanelFrame.repaint();
      }
      catch (CommunicationException e)
      {
         LOGGER.log(Level.SEVERE, "Communication Error", new Object[]
         {
            e,
            true
         });
      }
   }
}
