#include <mathwidgets.h>

N::DecisionEditor:: DecisionEditor ( QWidget * parent ,Plan * p              )
                  : TableWidget    (           parent ,       p              )
                  , Ownership      ( 0 , Types::None , Groups::Subordination )
                  , CodeManager    (                        p                )
                  , Uuid           ( 0                                       )
                  , Name           ( ""                                      )
                  , Changed        ( false                                   )
                  , Conditions     (  0                                      )
                  , Actions        (  0                                      )
                  , MaxConditions  ( 16                                      )
                  , Items          ( NULL                                    )
                  , AtRow          ( -1                                      )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::DecisionEditor::~DecisionEditor (void)
{
}

void N::DecisionEditor::contextMenuEvent(QContextMenuEvent * e)
{
  if ( Menu ( e -> pos ( ) ) ) e -> accept ( ) ;
  else QTableWidget::contextMenuEvent ( e )    ;
}

bool N::DecisionEditor::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone  ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll   ( ) ) ;
  LinkAction        ( Font         , setFont     ( ) ) ;
  LinkAction        ( Delete       , Delete      ( ) ) ;
  LinkAction        ( Save         , Save        ( ) ) ;
  return true                                          ;
}

void N::DecisionEditor::Configure(void)
{
  setDragDropMode              ( NoDragDrop            ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  ////////////////////////////////////////////////////////
  Dimension                    ( 1,1                   ) ;
  plan->setFont                ( this                  ) ;
  ////////////////////////////////////////////////////////
  QTableWidgetItem * cell                                ;
  cell  = new QTableWidgetItem (                       ) ;
  cell -> setText              ( tr("Condition" )      ) ;
  cell -> setData              ( Qt::UserRole,0        ) ;
  setItem                      ( 0 , 0 , cell          ) ;
  cell  = new QTableWidgetItem (                       ) ;
  cell -> setText              ( tr("Action")          ) ;
  cell -> setData              ( Qt::UserRole , 0      ) ;
  setItem                      ( 2 , 0 , cell          ) ;
  ////////////////////////////////////////////////////////
  nConnect ( this , SIGNAL(cellChanged      (int,int))   ,
             this , SLOT  (CellChanged      (int,int)) ) ;
  nConnect ( this , SIGNAL(cellPressed      (int,int))   ,
             this , SLOT  (CellPressed      (int,int)) ) ;
  nConnect ( this , SIGNAL(cellDoubleClicked(int,int))   ,
             this , SLOT  (CellDoubleClicked(int,int)) ) ;
}

void N::DecisionEditor::run(int Type,ThreadData * data)
{
}

bool N::DecisionEditor::startup(SUID uuid)
{
  blockSignals ( true  )                            ;
  EnterSQL ( SC , plan->sql )                       ;
    QString Q                                       ;
    QString name                                    ;
    UUIDs   CondUuids                               ;
    UUIDs   ActionUuids                             ;
    /////////////////////////////////////////////////
    Uuid = uuid                                     ;
    Name = SC.getName(PlanTable(Names)              ,
             "uuid",vLanguageId,uuid              ) ;
    setWindowTitle(Name)                            ;
    /////////////////////////////////////////////////
    Q = SC.sql.SelectFrom                           (
          "second"                                  ,
          PlanTable(Groups)                         ,
          QString("where first = %1 and "
                  "t1 = %2 and "
                  "relation = %3 "
                  "order by position asc"           )
          . arg ( Uuid                              )
          . arg ( Types::DecisionTable              )
          . arg ( Groups::Action                ) ) ;
    SqlLoopNow ( SC , Q )                           ;
      ActionUuids << SC . Uuid ( 0 )                ;
    SqlLoopErr ( SC , Q )                           ;
    SqlLoopEnd ( SC , Q )                           ;
    /////////////////////////////////////////////////
    Q = SC.sql.SelectFrom                           (
          "second"                                  ,
          PlanTable(Groups)                         ,
          QString("where first = %1 and "
                  "t1 = %2 and "
                  "relation = %3 "
                  "order by position asc"           )
          . arg ( Uuid                              )
          . arg ( Types::DecisionTable              )
          . arg ( Groups::Condition             ) ) ;
    SqlLoopNow ( SC , Q )                           ;
      CondUuids << SC . Uuid ( 0 )                  ;
    SqlLoopErr ( SC , Q )                           ;
    SqlLoopEnd ( SC , Q )                           ;
    /////////////////////////////////////////////////
    if ( CondUuids   . count ( ) > 0               &&
         ActionUuids . count ( ) > 0  )             {
      Dimension                                     (
        CondUuids   . count()                       ,
        ActionUuids . count()                     ) ;
      ///////////////////////////////////////////////
      SUID  muid                                    ;
      int   RowId = 0                               ;
      ///////////////////////////////////////////////
      foreach (muid,CondUuids)                      {
        name = CodeManager::Name                    (
                 SC,muid,vLanguageId              ) ;
        QTableWidgetItem * cell                     ;
        cell  = new QTableWidgetItem()              ;
        cell -> setData ( Qt::UserRole,muid )       ;
        cell -> setText ( name              )       ;
        setItem         ( RowId , 0 , cell  )       ;
        RowId++                                     ;
      }                                             ;
      ///////////////////////////////////////////////
      RowId++                                       ;
      ///////////////////////////////////////////////
      foreach ( muid , ActionUuids )                {
        name = CodeManager::Name                    (
                 SC,muid,vLanguageId              ) ;
        QTableWidgetItem * cell                     ;
        cell  = new QTableWidgetItem()              ;
        cell -> setData ( Qt::UserRole,muid )       ;
        cell -> setText ( name              )       ;
        setItem         ( RowId , 0 , cell  )       ;
        RowId++                                     ;
      }                                             ;
      ///////////////////////////////////////////////
      RowId = CondUuids.count() + 1                 ;
      ///////////////////////////////////////////////
      QList<int> Combines                           ;
      foreach ( muid , ActionUuids )                {
        Combines . clear ( )                        ;
        Q = SC . sql . SelectFrom                   (
              "combine"                             ,
              PlanTable(DecisionBranches)           ,
              QString("where decision = %1 and "
                      "action = %2 "
                      "order by combine asc"        )
              .arg(Uuid).arg(muid)                ) ;
        SqlLoopNow ( SC , Q )                       ;
          int v = SC.Int(0)                         ;
          if ( ! Combines . contains ( v ) )        {
            Combines << v                           ;
          }                                         ;
        SqlLoopErr ( SC , Q )                       ;
        SqlLoopEnd ( SC , Q )                       ;
        int cpos = 0                                ;
        int maxcolumns = 1                          ;
        maxcolumns <<= CondUuids.count()            ;
        int combine                                 ;
        foreach (combine,Combines)                  {
          int v = 0                                 ;
          cpos  = 0                                 ;
          Q = SC.sql.SelectFrom                     (
              "requirement,value"                   ,
              PlanTable(DecisionBranches)           ,
              QString("where decision = %1 and "
                      "action = %2 and "
                      "combine = %3"                )
              .arg(Uuid                             )
              .arg(muid                             )
              .arg(combine)                       ) ;
          SqlLoopNow ( SC , Q )                     ;
            SUID cuid = SC . Uuid ( 0 )             ;
                 v    = SC . Int  ( 1 )             ;
            if (v>0)                                {
              int shift = 0                         ;
              int z     = 1                         ;
              shift = CondUuids.indexOf(cuid)       ;
              shift = CondUuids.count() - shift - 1 ;
              if (shift>0) z <<= shift              ;
              cpos += z                             ;
            }                                       ;
          SqlLoopErr ( SC , Q )                     ;
          SqlLoopEnd ( SC , Q )                     ;
          if (cpos<maxcolumns)                      {
            QTableWidgetItem * a                    ;
            a  = new QTableWidgetItem()             ;
            a -> setText ( tr("Run")        )       ;
            a -> setData ( Qt::UserRole , 1 )       ;
            setItem ( RowId , maxcolumns-cpos , a ) ;
          }                                         ;
        }                                           ;
        RowId++                                     ;
      }                                             ;
    }                                               ;
  LeaveSQL ( SC , plan->sql )                       ;
  blockSignals ( false )                            ;
  ///////////////////////////////////////////////////
  resizeColumnToContents ( 0 )                      ;
  resizeRowsToContents   (   )                      ;
  ///////////////////////////////////////////////////
  Alert ( Done )                                    ;
  return true                                       ;
}

void N::DecisionEditor::Dimension(int conditions,int actions)
{
  int rows    = conditions + actions + 2                ;
  int columns = 1                                       ;
  columns <<= conditions                                ;
  setColumnCount ( columns + 1 )                        ;
  setRowCount    ( rows        )                        ;
  QTableWidgetItem * cell                               ;
  cell = new QTableWidgetItem()                         ;
  cell->setText(tr("Name"))                             ;
  setHorizontalHeaderItem ( 0 , cell )                  ;
  for (int i=1;i<=columns;i++)                          {
    cell = new QTableWidgetItem()                       ;
    cell->setText(QString::number(i))                   ;
    setHorizontalHeaderItem ( i , cell )                ;
  }                                                     ;
  for (int j=0;j<conditions;j++)                        {
    int gaps = conditions - j                           ;
    int mask = 1                                        ;
    mask <<= gaps                                       ;
    mask >>= 1                                          ;
    for (int i=1;i<=columns;i++)                        {
      bool Boolean = false                              ;
      int  Divide  = (i-1) / mask                       ;
      Boolean = ((Divide % 2) == 0)                     ;
      cell = new QTableWidgetItem(                    ) ;
      if (!Boolean)                                     {
        cell->setBackground(QColor(224,224,224))        ;
        cell->setForeground(QColor(255,  0,  0))        ;
      } else                                            {
        cell->setForeground(QColor(  0,  0,255))        ;
      }                                                 ;
      cell->setText(Boolean ? tr("True") : tr("False")) ;
      cell->setData  ( Qt::UserRole,Boolean ? 1 : 0 )   ;
      cell->setFlags ( Qt::NoItemFlags              )   ;
      setItem ( j , i , cell )                          ;
    }                                                   ;
  }                                                     ;
  for (int i=1;i<=columns;i++)                          {
    cell = new QTableWidgetItem(                      ) ;
    cell->setText  ( ""                               ) ;
    cell->setData  ( Qt::UserRole,-1                  ) ;
    cell->setFlags ( Qt::NoItemFlags                  ) ;
    cell->setBackgroundColor(QColor(240,240,240)      ) ;
    setItem ( conditions , i , cell                   ) ;
    cell = new QTableWidgetItem(                      ) ;
    cell->setText  ( ""                               ) ;
    cell->setData  ( Qt::UserRole,-1                  ) ;
    cell->setFlags ( Qt::NoItemFlags                  ) ;
    cell->setBackgroundColor(QColor(240,240,240)      ) ;
    setItem ( conditions+actions+1 , i , cell         ) ;
  }                                                     ;
  for (int i=1;i<=columns;i++)                          {
    resizeColumnToContents(i)                           ;
  }                                                     ;
  Conditions = conditions                               ;
  Actions    = actions                                  ;
}

void N::DecisionEditor::Delete(void)
{
  int column = currentColumn ( )          ;
  int row    = currentRow    ( )          ;
  if (column < 0                 ) return ;
  if (row    < 0                 ) return ;
  if (row== Conditions           ) return ;
  if (row==(Conditions+Actions+1)) return ;
  if (row < Conditions)                   {
    if (column!=0) return                 ;
    DeleteCondition ( )                   ;
  } else                                  {
    if (column==0) DeleteAction() ; else  {
      QTableWidgetItem * cell             ;
      blockSignals    ( true            ) ;
      cell  = new QTableWidgetItem (    ) ;
      cell -> setData ( Qt::UserRole,0  ) ;
      cell -> setText ( ""              ) ;
      setItem         ( row,column,cell ) ;
      blockSignals    ( false           ) ;
      Save            ( row , column    ) ;
    }                                     ;
  }                                       ;
}

void N::DecisionEditor::DeleteCondition(void)
{
  int row = currentRow (       ) ;
  Conditions--                   ;
  blockSignals         ( true  ) ;
  removeRow            ( row   ) ;
  blockSignals         ( false ) ;
  Inserted             (       ) ;
  Save                 (       ) ;
}

void N::DecisionEditor::DeleteAction(void)
{
  int row = currentRow (       ) ;
  Actions--                      ;
  blockSignals         ( true  ) ;
  removeRow            ( row   ) ;
  blockSignals         ( false ) ;
  Save                 (       ) ;
}

void N::DecisionEditor::ClearActions(void)
{
  int                columns = 1         ;
  columns <<= Conditions                 ;
  blockSignals ( true  )                 ;
  for (int i=0;i<Actions;i++)            {
    for (int j=1;j<=columns;j++)         {
      QTableWidgetItem * cell            ;
      cell  = new QTableWidgetItem ( )   ;
      cell -> setData ( Qt::UserRole,0 ) ;
      cell -> setText ( ""             ) ;
      setItem  ( i+Conditions+1,j,cell ) ;
    }                                    ;
  }                                      ;
  blockSignals ( false )                 ;
}

void N::DecisionEditor::CellChanged(int row,int column)
{
  if (column!=0) return                             ;
  QTableWidgetItem * cell    = NULL                 ;
  QTableWidgetItem * blank   = NULL                 ;
  int                columns = 1                    ;
  bool               actual  = false                ;
  columns <<= Conditions                            ;
  cell  = item ( row , column )                     ;
  if (IsNull(cell)) return                          ;
  if (cell->text().length()<=0) return              ;
  if (row< Conditions)                              { // modify condition
    resizeColumnToContents(0)                       ;
    Save ( )                                        ;
  } else
  if (row==Conditions)                              { // add condition
    blockSignals ( true  )                          ;
    blank = new QTableWidgetItem()                  ;
    insertRow ( row+1                )              ;
    setItem   ( row+1,column,blank   )              ;
    Conditions++                                    ;
    blockSignals ( false  )                         ;
    Inserted     (        )                         ;
    ClearActions (        )                         ;
    Save         (        )                         ;
  } else
  if (row==(Conditions+Actions+1))                  { // add action
    blockSignals ( true  )                          ;
    blank = new QTableWidgetItem()                  ;
    insertRow ( row+1                )              ;
    setItem   ( row+1,column,blank   )              ;
    Actions++                                       ;
    for (int i=1;i<=columns;i++)                    {
      cell = new QTableWidgetItem(                ) ;
      cell->setText  ( ""                         ) ;
      cell->setData  ( Qt::UserRole,0             ) ;
      setItem ( row        , i , cell             ) ;
      cell = new QTableWidgetItem(                ) ;
      cell->setText  ( ""                         ) ;
      cell->setData  ( Qt::UserRole,-1            ) ;
      cell->setFlags ( Qt::NoItemFlags            ) ;
      cell->setBackgroundColor(QColor(240,240,240)) ;
      setItem ( Conditions , i , cell             ) ;
      cell = new QTableWidgetItem(                ) ;
      cell->setText  ( ""                         ) ;
      cell->setData  ( Qt::UserRole,-1            ) ;
      cell->setFlags ( Qt::NoItemFlags            ) ;
      cell->setBackgroundColor(QColor(240,240,240)) ;
      setItem ( Conditions+Actions+1 , i , cell   ) ;
    }                                               ;
    blockSignals ( false  )                         ;
    Save         (        )                         ;
  } else                                            { // modify condition
    blockSignals ( true  )                          ;
    if (IsNull(cell)) actual = false ; else         {
      actual = (cell->text().length()>0)            ;
    }                                               ;
    actual = actual ? false : true                  ;
    if (actual)                                     {
      cell  = new QTableWidgetItem()                ;
      cell -> setText ( tr("Run")      )            ;
      cell -> setData ( Qt::UserRole,1 )            ;
    } else                                          {
      cell -> setText ( ""             )            ;
      cell -> setData ( Qt::UserRole,0 )            ;
    }                                               ;
    setItem      ( row,column,cell )                ;
    blockSignals ( false  )                         ;
    resizeColumnToContents ( 0 )                    ;
    Save                   (   )                    ;
  }                                                 ;
}

void N::DecisionEditor::CellPressed(int row,int column)
{
  if (row< Conditions)             {
    if (column==0)                 {
    }                              ;
  } else
  if (row== Conditions           ) {
  } else
  if (row==(Conditions+Actions+1)) {
  } else                           {
    if (column==0)                 {
    }                              ;
  }                                ;
}

void N::DecisionEditor::CellDoubleClicked(int row,int column)
{
  if (AtRow>=0)                                            {
    removeCellWidget ( AtRow , 0 )                         ;
    AtRow = -1                                             ;
    Items = NULL                                           ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  if (row<  Conditions           )                         {
    if (column>0)                                          {
    } else                                                 {
      Mount ( row , 0 )                                    ;
    }                                                      ;
  } else
  if (row== Conditions           )                         {
    if (column>0)                                          {
    } else                                                 {
      Mount ( row , 1 )                                    ;
    }                                                      ;
  } else
  if (row==(Conditions+Actions+1))                         {
    if (column>0)                                          {
    } else                                                 {
      Mount ( row , 3 )                                    ;
    }                                                      ;
  } else                                                   { // switch execution
    if (column>0)                                          {
      blockSignals ( true  )                               ;
      bool actual = false                                  ;
      QTableWidgetItem * cell                              ;
      cell = item(row,column)                              ;
      if (IsNull(cell)) actual = false                     ;
                   else actual = (cell->text().length()>0) ;
      actual = actual ? false : true                       ;
      if (actual)                                          {
        cell = new QTableWidgetItem()                      ;
        cell->setText(tr("Run"))                           ;
      } else cell = NULL                                   ;
      setItem ( row , column , cell )                      ;
      blockSignals ( false  )                              ;
      Save    ( row , column        )                      ;
    } else                                                 {
      Mount ( row , 2 )                                    ;
    }                                                      ;
  }                                                        ;
}

void N::DecisionEditor::Inserted(void)
{
  blockSignals ( true  )                                ;
  ///////////////////////////////////////////////////////
  int rows    = Conditions + Actions + 2                ;
  int columns = 1                                       ;
  ///////////////////////////////////////////////////////
  columns <<= Conditions                                ;
  setColumnCount          ( columns+1 )                 ;
  ///////////////////////////////////////////////////////
  QTableWidgetItem * cell                               ;
  cell = new QTableWidgetItem()                         ;
  cell->setText(tr("Name"))                             ;
  setHorizontalHeaderItem ( 0 , cell  )                 ;
  ///////////////////////////////////////////////////////
  for (int i=1;i<=columns;i++)                          {
    cell = new QTableWidgetItem()                       ;
    cell->setText(QString::number(i))                   ;
    setHorizontalHeaderItem ( i , cell )                ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int j=0;j<Conditions;j++)                        {
    int gaps = Conditions - j                           ;
    int mask = 1                                        ;
    mask <<= gaps                                       ;
    mask >>= 1                                          ;
    for (int i=1;i<=columns;i++)                        {
      bool Boolean = false                              ;
      int  Divide  = (i-1) / mask                       ;
      Boolean = ((Divide % 2) == 0)                     ;
      cell = new QTableWidgetItem(                    ) ;
      if (!Boolean)                                     {
        cell->setBackground(QColor(224,224,224))        ;
        cell->setForeground(QColor(255,  0,  0))        ;
      } else                                            {
        cell->setForeground(QColor(  0,  0,255))        ;
      }                                                 ;
      cell->setText(Boolean ? tr("True") : tr("False")) ;
      cell -> setData  ( Qt::UserRole,Boolean ? 1 : 0 ) ;
      cell -> setFlags ( Qt::NoItemFlags              ) ;
      setItem          (j,i,cell                      ) ;
    }                                                   ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=1;i<=columns;i++)                          {
    cell  = new QTableWidgetItem(                     ) ;
    cell -> setText  ( ""                             ) ;
    cell -> setData  ( Qt::UserRole,-1                ) ;
    cell -> setFlags ( Qt::NoItemFlags                ) ;
    cell -> setBackgroundColor (QColor(240,240,240)   ) ;
    setItem          ( Conditions,i,cell              ) ;
    cell  = new QTableWidgetItem(                     ) ;
    cell -> setText  ( ""                             ) ;
    cell -> setData  ( Qt::UserRole,-1                ) ;
    cell -> setFlags ( Qt::NoItemFlags                ) ;
    cell -> setBackgroundColor (QColor(240,240,240)   ) ;
    setItem          ( Conditions+Actions+1,i,cell    ) ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=1;i<=columns;i++)                          {
    resizeColumnToContents ( i )                        ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  blockSignals ( false )                                ;
}

void N::DecisionEditor::Mount(int row,int type)
{
  QTableWidgetItem * cell                            ;
  UUIDs   Uuids                                      ;
  UUIDs   Cuids                                      ;
  UUIDs   Auids                                      ;
  SUID    uuid                                       ;
  SUID    muid                                       ;
  QString name                                       ;
  int     index = -1                                 ;
  int     j     =  0                                 ;
  for (int i=0;i<Conditions;i++)                     {
    cell   = item       ( i , 0 )                    ;
    if (NotNull(cell))                               {
      muid   = nTableUuid ( cell  )                  ;
      if (muid>0) Cuids << muid                      ;
    }                                                ;
  }                                                  ;
  for (int i=0;i<Actions   ;i++)                     {
    cell   = item       ( Conditions + i + 1 , 0 )   ;
    if (NotNull(cell))                               {
      muid   = nTableUuid ( cell  )                  ;
      if (muid>0) Auids << muid                      ;
    }                                                ;
  }                                                  ;
  cell          = item ( row , 0 )                   ;
  if (NotNull(cell)) muid = nTableUuid(cell)         ;
  ////////////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                        ;
  switch (type)                                      {
    case 0                                           : // Modify Condition
      if (muid>0)                                    {
        j = Cuids . indexOf ( muid )                 ;
        if (j>=0) Cuids . takeAt ( j )               ;
      }                                              ;
      cell  = item ( row , 0 )                       ;
      muid  = cell->data(Qt::UserRole).toULongLong() ;
      Items = new QComboBox( )                       ;
      AtRow = row                                    ;
      Uuids = SC.Uuids(PlanTable(Conditions)         ,
              "uuid","order by id asc"             ) ;
      j     = 0                                      ;
      for (int i=0;i<Uuids.count();i++)              {
        uuid   = Uuids [ i ]                         ;
        if (!Cuids.contains(uuid))                   {
          name   = CodeManager::Name                 (
                   SC,uuid,vLanguageId             ) ;
          Items -> addItem ( name , uuid )           ;
          if (uuid==muid) index = j                  ;
          j++                                        ;
        }                                            ;
      }                                              ;
      if (index>=0) Items->setCurrentIndex(index)    ;
      connect(Items,SIGNAL(activated      (int))     ,
              this ,SLOT  (ModifyCondition(int))   ) ;
    break                                            ;
    case 1                                           : // Add Condition
      Items = new QComboBox( )                       ;
      AtRow = row                                    ;
      Uuids = SC.Uuids(PlanTable(Conditions)         ,
              "uuid","order by id asc"             ) ;
      for (int i=0;i<Uuids.count();i++)              {
        uuid   = Uuids [ i ]                         ;
        if (!Cuids.contains(uuid))                   {
          name   = CodeManager::Name                 (
                   SC,uuid,vLanguageId             ) ;
          Items -> addItem ( name , uuid )           ;
        }                                            ;
      }                                              ;
      connect(Items,SIGNAL(activated      (int))     ,
              this ,SLOT  (AddCondition   (int))   ) ;
    break                                            ;
    case 2                                           : // Modify Action
      if (muid>0)                                    {
        j = Auids . indexOf ( muid )                 ;
        if (j>=0) Auids . takeAt ( j )               ;
      }                                              ;
      cell  = item ( row , 0 )                       ;
      muid  = cell->data(Qt::UserRole).toULongLong() ;
      Items = new QComboBox( )                       ;
      AtRow = row                                    ;
      Uuids = SC.Uuids(PlanTable(Actions)            ,
              "uuid","order by id asc"             ) ;
      j     = 0                                      ;
      for (int i=0;i<Uuids.count();i++)              {
        uuid   = Uuids [ i ]                         ;
        if (!Auids.contains(uuid))                   {
          name   = CodeManager::Name                 (
                   SC,uuid,vLanguageId             ) ;
          Items -> addItem ( name , uuid )           ;
          if (uuid==muid) index = j                  ;
          j++                                        ;
        }                                            ;
      }                                              ;
      if (index>=0) Items->setCurrentIndex(index)    ;
      connect(Items,SIGNAL(activated      (int))     ,
              this ,SLOT  (ModifyAction   (int))   ) ;
    break                                            ;
    case 3                                           : // Add Action
      Items = new QComboBox( )                       ;
      AtRow = row                                    ;
      Uuids = SC.Uuids(PlanTable(Actions)            ,
              "uuid","order by id asc"             ) ;
      for (int i=0;i<Uuids.count();i++)              {
        uuid   = Uuids [ i ]                         ;
        name   = CodeManager::Name                   (
                 SC,uuid,vLanguageId               ) ;
        Items -> addItem ( name , uuid )             ;
      }                                              ;
      connect(Items,SIGNAL(activated      (int))     ,
              this ,SLOT  (AddAction      (int))   ) ;
    break                                            ;
  }                                                  ;
  ErrorSQL ( SC , plan->sql )                        ;
  LeaveSQL ( SC , plan->sql )                        ;
  ////////////////////////////////////////////////////
  if ( ( AtRow >= 0 ) && NotNull(Items) )            {
    Items -> setEditable        ( true             ) ;
    Items -> setMaxVisibleItems ( 30               ) ;
    setCellWidget               ( row , 0 , Items  ) ;
    Items -> showPopup          (                  ) ;
  }                                                  ;
}

void N::DecisionEditor::ModifyCondition(int)
{
  if ( AtRow < 0     ) return                          ;
  if ( IsNull(Items) ) return                          ;
  //////////////////////////////////////////////////////
  QString name = Items -> currentText ( )              ;
  int     Row  = AtRow                                 ;
  SUID    muid = N::GetUuid(Items)                     ;
  //////////////////////////////////////////////////////
  removeCellWidget             ( AtRow , 0           ) ;
  AtRow = -1                                           ;
  Items = NULL                                         ;
  //////////////////////////////////////////////////////
  QTableWidgetItem * cell                              ;
  cell  = new QTableWidgetItem (                     ) ;
  cell -> setData              ( Qt::UserRole , muid ) ;
  cell -> setText              ( name                ) ;
  setItem                      ( Row , 0 , cell      ) ;
}

void N::DecisionEditor::AddCondition(int)
{
  if ( AtRow < 0     ) return                          ;
  if ( IsNull(Items) ) return                          ;
  //////////////////////////////////////////////////////
  QString name = Items -> currentText ( )              ;
  int     Row  = AtRow                                 ;
  SUID    muid = N::GetUuid(Items)                     ;
  //////////////////////////////////////////////////////
  removeCellWidget             ( AtRow , 0           ) ;
  AtRow = -1                                           ;
  Items = NULL                                         ;
  //////////////////////////////////////////////////////
  QTableWidgetItem * cell                              ;
  cell  = new QTableWidgetItem (                     ) ;
  cell -> setData              ( Qt::UserRole , muid ) ;
  cell -> setText              ( name                ) ;
  setItem                      ( Row , 0 , cell      ) ;
}

void N::DecisionEditor::ModifyAction(int)
{
  if ( AtRow < 0     ) return                          ;
  if ( IsNull(Items) ) return                          ;
  //////////////////////////////////////////////////////
  QString name = Items -> currentText ( )              ;
  int     Row  = AtRow                                 ;
  SUID    muid = N::GetUuid(Items)                     ;
  //////////////////////////////////////////////////////
  removeCellWidget             ( AtRow , 0           ) ;
  AtRow = -1                                           ;
  Items = NULL                                         ;
  //////////////////////////////////////////////////////
  QTableWidgetItem * cell                              ;
  cell  = new QTableWidgetItem (                     ) ;
  cell -> setData              ( Qt::UserRole , muid ) ;
  cell -> setText              ( name                ) ;
  setItem                      ( Row , 0 , cell      ) ;
}

void N::DecisionEditor::AddAction(int)
{
  if ( AtRow < 0     ) return                          ;
  if ( IsNull(Items) ) return                          ;
  //////////////////////////////////////////////////////
  QString name = Items -> currentText ( )              ;
  int     Row  = AtRow                                 ;
  SUID    muid = N::GetUuid(Items)                     ;
  //////////////////////////////////////////////////////
  removeCellWidget             ( AtRow , 0           ) ;
  AtRow = -1                                           ;
  Items = NULL                                         ;
  //////////////////////////////////////////////////////
  QTableWidgetItem * cell                              ;
  cell  = new QTableWidgetItem (                     ) ;
  cell -> setData              ( Qt::UserRole , muid ) ;
  cell -> setText              ( name                ) ;
  setItem                      ( Row , 0 , cell      ) ;
}

QSize N::DecisionEditor::PreferSize (void)
{
  QSize S(80,60)                     ;
  int RowId    = rowCount    ( ) - 1 ;
  int ColumnId = columnCount ( ) - 1 ;
  if (RowId    < 0) return S         ;
  if (ColumnId < 0) return S         ;
  QTableWidgetItem * cell            ;
  cell = item(RowId,ColumnId)        ;
  if (IsNull(cell)) return S         ;
  QRect r = visualItemRect(cell)     ;
  int   W = r . right  ()            ;
  int   H = r . bottom ()            ;
  S . setWidth  ( W + 2 )            ;
  S . setHeight ( H + 2 )            ;
  return S                           ;
}

void N::DecisionEditor::AdjustSize(void)
{
  bool mdi = false                                        ;
  /////////////////////////////////////////////////////////
  QWidget       * pw  = parentWidget ( )                  ;
  QMdiSubWindow * msw = NULL                              ;
  QSize           f                                       ;
  QSize           x                                       ;
  QSize           v                                       ;
  if (NotNull(pw)) msw = Casting ( QMdiSubWindow , pw )   ;
  mdi = NotNull ( msw )                                   ;
  /////////////////////////////////////////////////////////
  if (mdi)                                                {
    f  = msw -> geometry ( ) . size ( )                   ;
    v  = size                       ( )                   ;
    f -= v                                                ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  QSize S = PreferSize()                                  ;
  QSize W ( S . width ( ) + 32 , S . height ( ) + 48 )    ;
  resize ( W )                                            ;
  /////////////////////////////////////////////////////////
  if (mdi)                                                {
    x  = W                                                ;
    x += f                                                ;
    msw -> resize ( x )                                   ;
  }                                                       ;
  /////////////////////////////////////////////////////////
}

bool N::DecisionEditor::Save(int,int)
{
  if ( Uuid <= 0 ) return false                       ;
  EnterSQL         ( SC , plan->sql          )        ;
    QString Q                                         ;
    SUID    muid                                      ;
    int     RowId    = 0                              ;
    UUIDs   ConditionUuids                            ;
    UUIDs   ActionUuids                               ;
    ///////////////////////////////////////////////////
    for (int i=0;i<Conditions;i++)                    {
      QTableWidgetItem * cell                         ;
      SUID               uuid                         ;
      cell = item       (RowId,0)                     ;
      uuid = nTableUuid (cell   )                     ;
      ConditionUuids <<  uuid                         ;
      RowId++                                         ;
    }                                                 ;
    ///////////////////////////////////////////////////
    RowId++                                           ;
    ///////////////////////////////////////////////////
    for (int i=0;i<Actions;i++)                       {
      QTableWidgetItem * cell                         ;
      SUID               uuid                         ;
      cell = item       (RowId,0)                     ;
      uuid = nTableUuid (cell   )                     ;
      ActionUuids    <<  uuid                         ;
      RowId++                                         ;
    }                                                 ;
    RowId = Conditions + 1                            ;
    ///////////////////////////////////////////////////
    Q = SC.sql.DeleteFrom                             (
          PlanTable(DecisionBranches)                 ,
          QString("where decision = %1").arg(Uuid) )  ;
    SC . Query ( Q )                                  ;
    ///////////////////////////////////////////////////
    int columns = 1                                   ;
    columns   <<= Conditions                          ;
    foreach (muid,ActionUuids)                        {
      int combine = 1                                 ;
      for (int i=1;i<=columns;i++)                    {
        QTableWidgetItem * cell = item(RowId,i)       ;
        if (NotNull(cell) && cell->text().length()>0) {
          for (int j=0;j<Conditions;j++)              {
            SUID cuid = ConditionUuids [ j ]          ;
            QTableWidgetItem * b = item(j,i)          ;
            if (NotNull(b))                           {
              int v = b->data(Qt::UserRole).toInt()   ;
              Q = SC.sql.InsertInto                   (
                    PlanTable(DecisionBranches)       ,
                    5                                 ,
                    "decision","action","requirement" ,
                    "combine","value"               ) ;
              SC.Prepare( Q                         ) ;
              SC.Bind   ( "decision"    , Uuid      ) ;
              SC.Bind   ( "action"      , muid      ) ;
              SC.Bind   ( "requirement" , cuid      ) ;
              SC.Bind   ( "combine"     , combine   ) ;
              SC.Bind   ( "value"       , v         ) ;
              SC.Exec   (                           ) ;
            }                                         ;
          }                                           ;
          combine++                                   ;
        }                                             ;
      }                                               ;
      RowId++                                         ;
    }                                                 ;
  LeaveSQL         ( SC , plan->sql          )        ;
  emit Updated     ( this                    )        ;
  Alert            ( Done                    )        ;
  return true                                         ;
}

bool N::DecisionEditor::Save(void)
{
  if ( Uuid <= 0 ) return false                             ;
  EnterSQL  ( SC , plan->sql )                              ;
    QString Q                                               ;
    SUID    muid                                            ;
    int     RowId    = 0                                    ;
    int     Position = 0                                    ;
    UUIDs   ConditionUuids                                  ;
    UUIDs   ActionUuids                                     ;
    /////////////////////////////////////////////////////////
    for (int i=0;i<Conditions;i++)                          {
      QTableWidgetItem * cell                               ;
      SUID               uuid                               ;
      cell = item       (RowId,0)                           ;
      uuid = nTableUuid (cell   )                           ;
      ConditionUuids <<  uuid                               ;
      RowId++                                               ;
    }                                                       ;
    /////////////////////////////////////////////////////////
    RowId++                                                 ;
    /////////////////////////////////////////////////////////
    for (int i=0;i<Actions;i++)                             {
      QTableWidgetItem * cell                               ;
      SUID               uuid                               ;
      cell = item       (RowId,0)                           ;
      uuid = nTableUuid (cell   )                           ;
      ActionUuids    <<  uuid                               ;
      RowId++                                               ;
    }                                                       ;
    RowId = Conditions + 1                                  ;
    /////////////////////////////////////////////////////////
    Q = SC . sql . DeleteFrom ( PlanTable(Groups)           ,
          QString("where first = %1 and"
                  " t1 = %2 and relation = %3"              )
          . arg ( Uuid                                      )
          . arg ( Types::DecisionTable                      )
          . arg ( Groups::Action                        ) ) ;
    SC . Query ( Q )                                        ;
    Q = SC . sql . DeleteFrom                               (
          PlanTable(Groups)                                 ,
          QString("where first = %1 and"
                  " t1 = %2 and relation = %3"              )
          . arg ( Uuid                                      )
          . arg ( Types::DecisionTable                      )
          . arg ( Groups::Condition)                      ) ;
    SC . Query ( Q )                                        ;
    Q = SC . sql . DeleteFrom                               (
          PlanTable(DecisionBranches)                       ,
          QString("where decision = %1").arg(Uuid)        ) ;
    SC . Query ( Q )                                        ;
    /////////////////////////////////////////////////////////
    RMAPs ATypes                                            ;
    for (int i=0;i<ActionUuids.count();i++)                 {
      SUID uuid = ActionUuids [ i ]                         ;
      Q = SC . sql . SelectFrom                             (
            "type"                                          ,
            PlanTable(MajorUuid)                            ,
            SC . WhereUuid ( uuid )                       ) ;
      if (SC.Fetch(Q))                                      {
        ATypes [uuid] = (int)SC.Uuid(0)                     ;
      } else ATypes[uuid] = 0                               ;
    }                                                       ;
    Position = 1                                            ;
    for (int i=0;i<ActionUuids.count();i++)                 {
      SUID uuid = ActionUuids [ i ]                         ;
      Q = SC . sql . InsertInto                             (
            PlanTable(Groups)                               ,
            6                                               ,
            "first"                                         ,
            "second"                                        ,
            "t1"                                            ,
             "t2"                                           ,
            "relation"                                      ,
            "position"                                    ) ;
      SC . Prepare ( Q                                    ) ;
      SC . Bind    ( "first"    , Uuid                    ) ;
      SC . Bind    ( "second"   , uuid                    ) ;
      SC . Bind    ( "t1"       , Types::DecisionTable    ) ;
      SC . Bind    ( "t2"       , ATypes[uuid]            ) ;
      SC . Bind    ( "relation" , Groups::Action          ) ;
      SC . Bind    ( "position" , Position                ) ;
      SC . Exec    (                                      ) ;
      Position ++                                           ;
    }                                                       ;
    /////////////////////////////////////////////////////////
    Position = 1                                            ;
    for (int i = 0 ; i < ConditionUuids . count ( ) ; i++ ) {
      SUID uuid = ConditionUuids [ i ]                      ;
      Q  = SC . sql . InsertInto                            (
             PlanTable(Groups)                              ,
             6                                              ,
             "first"                                        ,
             "second"                                       ,
             "t1"                                           ,
             "t2"                                           ,
             "relation"                                     ,
             "position"                                )    ;
      SC . Prepare ( Q                                 )    ;
      SC . Bind    ( "first"    , Uuid                 )    ;
      SC . Bind    ( "second"   , uuid                 )    ;
      SC . Bind    ( "t1"       , Types::DecisionTable )    ;
      SC . Bind    ( "t2"       , Types::Condition     )    ;
      SC . Bind    ( "relation" , Groups::Condition    )    ;
      SC . Bind    ( "position" , Position             )    ;
      SC . Exec    (                                   )    ;
      Position ++                                           ;
    }                                                       ;
    /////////////////////////////////////////////////////////
    int columns = 1                                         ;
    columns <<= Conditions                                  ;
    foreach ( muid , ActionUuids )                          {
      int combine = 1                                       ;
      for (int i=1;i<=columns;i++)                          {
        QTableWidgetItem * cell = item(RowId,i)             ;
        if (NotNull(cell) && cell->text().length()>0)       {
          for (int j=0;j<Conditions;j++)                    {
            SUID cuid = ConditionUuids [ j ]                ;
            QTableWidgetItem * b = item(j,i)                ;
            if (NotNull(b))                                 {
              int v = b->data(Qt::UserRole).toInt()         ;
              Q = SC . sql . InsertInto                     (
                    PlanTable(DecisionBranches)             ,
                    5                                       ,
                    "decision"                              ,
                    "action"                                ,
                    "requirement"                           ,
                    "combine"                               ,
                    "value"                            )    ;
              SC . Prepare ( Q                         )    ;
              SC . Bind    ( "decision"    , Uuid      )    ;
              SC . Bind    ( "action"      , muid      )    ;
              SC . Bind    ( "requirement" , cuid      )    ;
              SC . Bind    ( "combine"     , combine   )    ;
              SC . Bind    ( "value"       , v         )    ;
              SC . Exec    (                           )    ;
            }                                               ;
          }                                                 ;
          combine++                                         ;
        }                                                   ;
      }                                                     ;
      RowId++                                               ;
    }                                                       ;
  LeaveSQL     ( SC , plan->sql )                           ;
  emit Updated ( this           )                           ;
  Alert        ( Done           )                           ;
  return true                                               ;
}

bool N::DecisionEditor::Menu(QPoint pos)
{
  return true ;
}
