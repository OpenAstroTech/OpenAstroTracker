namespace OpenAstroTracker_Control
{
    partial class Form1
    {
        /// <summary>
        /// Erforderliche Designervariable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Verwendete Ressourcen bereinigen.
        /// </summary>
        /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Vom Windows Form-Designer generierter Code

        /// <summary>
        /// Erforderliche Methode für die Designerunterstützung.
        /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
        /// </summary>
        private void InitializeComponent()
        {
			this.cbPorts = new System.Windows.Forms.ComboBox();
			this.button1 = new System.Windows.Forms.Button();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.btnGetCurrent = new System.Windows.Forms.Button();
			this.comboBox2 = new System.Windows.Forms.ComboBox();
			this.udDecSeconds = new System.Windows.Forms.NumericUpDown();
			this.udRASeconds = new System.Windows.Forms.NumericUpDown();
			this.udDecMinutes = new System.Windows.Forms.NumericUpDown();
			this.udRAMinutes = new System.Windows.Forms.NumericUpDown();
			this.udDecDegrees = new System.Windows.Forms.NumericUpDown();
			this.label10 = new System.Windows.Forms.Label();
			this.udRAHours = new System.Windows.Forms.NumericUpDown();
			this.label11 = new System.Windows.Forms.Label();
			this.btnDecMove = new System.Windows.Forms.Button();
			this.label12 = new System.Windows.Forms.Label();
			this.btnRAMove = new System.Windows.Forms.Button();
			this.label6 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.udManualSteps = new System.Windows.Forms.NumericUpDown();
			this.label4 = new System.Windows.Forms.Label();
			this.btnManualRight = new System.Windows.Forms.Button();
			this.btnManualLeft = new System.Windows.Forms.Button();
			this.btnManualDown = new System.Windows.Forms.Button();
			this.btnManualUp = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.udDecSeconds)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.udRASeconds)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.udDecMinutes)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.udRAMinutes)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.udDecDegrees)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.udRAHours)).BeginInit();
			this.groupBox2.SuspendLayout();
			this.groupBox3.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.udManualSteps)).BeginInit();
			this.SuspendLayout();
			// 
			// cbPorts
			// 
			this.cbPorts.FormattingEnabled = true;
			this.cbPorts.Location = new System.Drawing.Point(13, 30);
			this.cbPorts.Margin = new System.Windows.Forms.Padding(2);
			this.cbPorts.Name = "cbPorts";
			this.cbPorts.Size = new System.Drawing.Size(81, 21);
			this.cbPorts.TabIndex = 0;
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(105, 27);
			this.button1.Margin = new System.Windows.Forms.Padding(2);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(112, 28);
			this.button1.TabIndex = 1;
			this.button1.Text = "Connect";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox1.Controls.Add(this.btnGetCurrent);
			this.groupBox1.Controls.Add(this.comboBox2);
			this.groupBox1.Controls.Add(this.udDecSeconds);
			this.groupBox1.Controls.Add(this.udRASeconds);
			this.groupBox1.Controls.Add(this.udDecMinutes);
			this.groupBox1.Controls.Add(this.udRAMinutes);
			this.groupBox1.Controls.Add(this.udDecDegrees);
			this.groupBox1.Controls.Add(this.label10);
			this.groupBox1.Controls.Add(this.udRAHours);
			this.groupBox1.Controls.Add(this.label11);
			this.groupBox1.Controls.Add(this.btnDecMove);
			this.groupBox1.Controls.Add(this.label12);
			this.groupBox1.Controls.Add(this.btnRAMove);
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Location = new System.Drawing.Point(8, 7);
			this.groupBox1.Margin = new System.Windows.Forms.Padding(2);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Padding = new System.Windows.Forms.Padding(2);
			this.groupBox1.Size = new System.Drawing.Size(361, 125);
			this.groupBox1.TabIndex = 2;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Coordinates";
			// 
			// btnGetCurrent
			// 
			this.btnGetCurrent.Location = new System.Drawing.Point(4, 92);
			this.btnGetCurrent.Margin = new System.Windows.Forms.Padding(2);
			this.btnGetCurrent.Name = "btnGetCurrent";
			this.btnGetCurrent.Size = new System.Drawing.Size(79, 25);
			this.btnGetCurrent.TabIndex = 5;
			this.btnGetCurrent.Text = "Get current";
			this.btnGetCurrent.UseVisualStyleBackColor = true;
			// 
			// comboBox2
			// 
			this.comboBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.comboBox2.FormattingEnabled = true;
			this.comboBox2.Location = new System.Drawing.Point(90, 95);
			this.comboBox2.Margin = new System.Windows.Forms.Padding(2);
			this.comboBox2.Name = "comboBox2";
			this.comboBox2.Size = new System.Drawing.Size(267, 21);
			this.comboBox2.TabIndex = 3;
			// 
			// udDecSeconds
			// 
			this.udDecSeconds.Location = new System.Drawing.Point(195, 56);
			this.udDecSeconds.Margin = new System.Windows.Forms.Padding(2);
			this.udDecSeconds.Maximum = new decimal(new int[] {
            59,
            0,
            0,
            0});
			this.udDecSeconds.Name = "udDecSeconds";
			this.udDecSeconds.Size = new System.Drawing.Size(46, 20);
			this.udDecSeconds.TabIndex = 25;
			// 
			// udRASeconds
			// 
			this.udRASeconds.Location = new System.Drawing.Point(195, 22);
			this.udRASeconds.Margin = new System.Windows.Forms.Padding(2);
			this.udRASeconds.Maximum = new decimal(new int[] {
            59,
            0,
            0,
            0});
			this.udRASeconds.Name = "udRASeconds";
			this.udRASeconds.Size = new System.Drawing.Size(46, 20);
			this.udRASeconds.TabIndex = 19;
			// 
			// udDecMinutes
			// 
			this.udDecMinutes.Location = new System.Drawing.Point(115, 56);
			this.udDecMinutes.Margin = new System.Windows.Forms.Padding(2);
			this.udDecMinutes.Maximum = new decimal(new int[] {
            59,
            0,
            0,
            0});
			this.udDecMinutes.Name = "udDecMinutes";
			this.udDecMinutes.Size = new System.Drawing.Size(46, 20);
			this.udDecMinutes.TabIndex = 24;
			// 
			// udRAMinutes
			// 
			this.udRAMinutes.Location = new System.Drawing.Point(115, 22);
			this.udRAMinutes.Margin = new System.Windows.Forms.Padding(2);
			this.udRAMinutes.Maximum = new decimal(new int[] {
            59,
            0,
            0,
            0});
			this.udRAMinutes.Name = "udRAMinutes";
			this.udRAMinutes.Size = new System.Drawing.Size(46, 20);
			this.udRAMinutes.TabIndex = 18;
			// 
			// udDecDegrees
			// 
			this.udDecDegrees.Location = new System.Drawing.Point(38, 56);
			this.udDecDegrees.Margin = new System.Windows.Forms.Padding(2);
			this.udDecDegrees.Maximum = new decimal(new int[] {
            90,
            0,
            0,
            0});
			this.udDecDegrees.Name = "udDecDegrees";
			this.udDecDegrees.Size = new System.Drawing.Size(45, 20);
			this.udDecDegrees.TabIndex = 23;
			// 
			// label10
			// 
			this.label10.AutoSize = true;
			this.label10.Location = new System.Drawing.Point(243, 58);
			this.label10.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(12, 13);
			this.label10.TabIndex = 22;
			this.label10.Text = "\"";
			// 
			// udRAHours
			// 
			this.udRAHours.Location = new System.Drawing.Point(38, 22);
			this.udRAHours.Margin = new System.Windows.Forms.Padding(2);
			this.udRAHours.Maximum = new decimal(new int[] {
            23,
            0,
            0,
            0});
			this.udRAHours.Name = "udRAHours";
			this.udRAHours.Size = new System.Drawing.Size(45, 20);
			this.udRAHours.TabIndex = 17;
			// 
			// label11
			// 
			this.label11.AutoSize = true;
			this.label11.Location = new System.Drawing.Point(163, 58);
			this.label11.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(9, 13);
			this.label11.TabIndex = 21;
			this.label11.Text = "\'";
			// 
			// btnDecMove
			// 
			this.btnDecMove.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.btnDecMove.Location = new System.Drawing.Point(281, 53);
			this.btnDecMove.Margin = new System.Windows.Forms.Padding(2);
			this.btnDecMove.Name = "btnDecMove";
			this.btnDecMove.Size = new System.Drawing.Size(76, 26);
			this.btnDecMove.TabIndex = 17;
			this.btnDecMove.Text = "Move";
			this.btnDecMove.UseVisualStyleBackColor = true;
			this.btnDecMove.Click += new System.EventHandler(this.btnDecMove_Click);
			// 
			// label12
			// 
			this.label12.AutoSize = true;
			this.label12.Location = new System.Drawing.Point(86, 58);
			this.label12.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
			this.label12.Name = "label12";
			this.label12.Size = new System.Drawing.Size(11, 13);
			this.label12.TabIndex = 20;
			this.label12.Text = "°";
			// 
			// btnRAMove
			// 
			this.btnRAMove.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.btnRAMove.Location = new System.Drawing.Point(281, 19);
			this.btnRAMove.Margin = new System.Windows.Forms.Padding(2);
			this.btnRAMove.Name = "btnRAMove";
			this.btnRAMove.Size = new System.Drawing.Size(76, 26);
			this.btnRAMove.TabIndex = 16;
			this.btnRAMove.Text = "Move";
			this.btnRAMove.UseVisualStyleBackColor = true;
			this.btnRAMove.Click += new System.EventHandler(this.btnRAMove_Click);
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(244, 24);
			this.label6.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(12, 13);
			this.label6.TabIndex = 13;
			this.label6.Text = "s";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(163, 24);
			this.label5.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(15, 13);
			this.label5.TabIndex = 11;
			this.label5.Text = "m";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(86, 24);
			this.label3.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(13, 13);
			this.label3.TabIndex = 5;
			this.label3.Text = "h";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(5, 58);
			this.label2.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(29, 13);
			this.label2.TabIndex = 1;
			this.label2.Text = "DEC";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(12, 24);
			this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(22, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "RA";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.cbPorts);
			this.groupBox2.Controls.Add(this.button1);
			this.groupBox2.Location = new System.Drawing.Point(7, 136);
			this.groupBox2.Margin = new System.Windows.Forms.Padding(2);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Padding = new System.Windows.Forms.Padding(2);
			this.groupBox2.Size = new System.Drawing.Size(231, 65);
			this.groupBox2.TabIndex = 3;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Connect";
			// 
			// groupBox3
			// 
			this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox3.Controls.Add(this.udManualSteps);
			this.groupBox3.Controls.Add(this.label4);
			this.groupBox3.Controls.Add(this.btnManualRight);
			this.groupBox3.Controls.Add(this.btnManualLeft);
			this.groupBox3.Controls.Add(this.btnManualDown);
			this.groupBox3.Controls.Add(this.btnManualUp);
			this.groupBox3.Location = new System.Drawing.Point(376, 7);
			this.groupBox3.Margin = new System.Windows.Forms.Padding(2);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Padding = new System.Windows.Forms.Padding(2);
			this.groupBox3.Size = new System.Drawing.Size(197, 304);
			this.groupBox3.TabIndex = 4;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Manual control";
			// 
			// udManualSteps
			// 
			this.udManualSteps.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.udManualSteps.Location = new System.Drawing.Point(109, 277);
			this.udManualSteps.Margin = new System.Windows.Forms.Padding(2);
			this.udManualSteps.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
			this.udManualSteps.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.udManualSteps.Name = "udManualSteps";
			this.udManualSteps.Size = new System.Drawing.Size(65, 20);
			this.udManualSteps.TabIndex = 16;
			this.udManualSteps.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
			// 
			// label4
			// 
			this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(26, 279);
			this.label4.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(79, 13);
			this.label4.TabIndex = 15;
			this.label4.Text = "Amount (Steps)";
			// 
			// btnManualRight
			// 
			this.btnManualRight.Image = global::OpenAstroTracker_Control.Properties.Resources.Arrow_right;
			this.btnManualRight.Location = new System.Drawing.Point(130, 80);
			this.btnManualRight.Margin = new System.Windows.Forms.Padding(2);
			this.btnManualRight.Name = "btnManualRight";
			this.btnManualRight.Size = new System.Drawing.Size(60, 60);
			this.btnManualRight.TabIndex = 3;
			this.btnManualRight.UseVisualStyleBackColor = true;
			this.btnManualRight.Click += new System.EventHandler(this.btnManualRight_Click);
			// 
			// btnManualLeft
			// 
			this.btnManualLeft.Image = global::OpenAstroTracker_Control.Properties.Resources.Arrow_left;
			this.btnManualLeft.Location = new System.Drawing.Point(10, 80);
			this.btnManualLeft.Margin = new System.Windows.Forms.Padding(2);
			this.btnManualLeft.Name = "btnManualLeft";
			this.btnManualLeft.Size = new System.Drawing.Size(60, 60);
			this.btnManualLeft.TabIndex = 2;
			this.btnManualLeft.UseVisualStyleBackColor = true;
			this.btnManualLeft.Click += new System.EventHandler(this.btnManualLeft_Click);
			// 
			// btnManualDown
			// 
			this.btnManualDown.Image = global::OpenAstroTracker_Control.Properties.Resources.Arrow_down;
			this.btnManualDown.Location = new System.Drawing.Point(70, 139);
			this.btnManualDown.Margin = new System.Windows.Forms.Padding(2);
			this.btnManualDown.Name = "btnManualDown";
			this.btnManualDown.Size = new System.Drawing.Size(60, 60);
			this.btnManualDown.TabIndex = 1;
			this.btnManualDown.UseVisualStyleBackColor = true;
			this.btnManualDown.Click += new System.EventHandler(this.btnManualDown_Click);
			// 
			// btnManualUp
			// 
			this.btnManualUp.Image = global::OpenAstroTracker_Control.Properties.Resources.Arrow_up;
			this.btnManualUp.Location = new System.Drawing.Point(70, 22);
			this.btnManualUp.Margin = new System.Windows.Forms.Padding(2);
			this.btnManualUp.Name = "btnManualUp";
			this.btnManualUp.Size = new System.Drawing.Size(60, 60);
			this.btnManualUp.TabIndex = 0;
			this.btnManualUp.UseVisualStyleBackColor = true;
			this.btnManualUp.Click += new System.EventHandler(this.btnManualUp_Click);
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(584, 322);
			this.Controls.Add(this.groupBox3);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Margin = new System.Windows.Forms.Padding(2);
			this.MaximizeBox = false;
			this.MinimumSize = new System.Drawing.Size(600, 300);
			this.Name = "Form1";
			this.Text = "OpenAstroTracker Control";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.udDecSeconds)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.udRASeconds)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.udDecMinutes)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.udRAMinutes)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.udDecDegrees)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.udRAHours)).EndInit();
			this.groupBox2.ResumeLayout(false);
			this.groupBox3.ResumeLayout(false);
			this.groupBox3.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.udManualSteps)).EndInit();
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ComboBox cbPorts;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button btnManualRight;
        private System.Windows.Forms.Button btnManualLeft;
        private System.Windows.Forms.Button btnManualDown;
        private System.Windows.Forms.Button btnManualUp;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.NumericUpDown udManualSteps;
        private System.Windows.Forms.Button btnDecMove;
        private System.Windows.Forms.Button btnRAMove;
        private System.Windows.Forms.NumericUpDown udRASeconds;
        private System.Windows.Forms.NumericUpDown udRAMinutes;
        private System.Windows.Forms.NumericUpDown udRAHours;
        private System.Windows.Forms.NumericUpDown udDecSeconds;
        private System.Windows.Forms.NumericUpDown udDecMinutes;
        private System.Windows.Forms.NumericUpDown udDecDegrees;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Button btnGetCurrent;
        private System.Windows.Forms.ComboBox comboBox2;
    }
}

