#include <mathwidgets.h>

N::ConditionLists:: ConditionLists ( QWidget * parent , Plan * p             )
                  : TreeDock       (           parent ,        p             )
                  , Ownership      ( 0 , Types::None , Groups::Subordination )
                  , GroupItems     (                           p             )
                  , Sorting        ( Qt::DescendingOrder                     )
                  , dropAction     ( false                                   )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ConditionLists::~ConditionLists(void)
{
}

QSize N::ConditionLists::sizeHint(void) const
{
  return SizeSuggestion ( QSize ( 640 , 480 ) ) ;
}

QMimeData * N::ConditionLists::dragMime (void)
{
  QTreeWidgetItem * IT = currentItem ( )     ;
  if (IsNull(IT)) return NULL                ;
  SUID        uuid = nTreeUuid ( IT , 0 )    ;
  QMimeData * mime = new QMimeData ( )       ;
  setMime ( mime , "condition/uuid" , uuid ) ;
  return mime                                ;
}

bool N::ConditionLists::hasItem(void)
{
  QTreeWidgetItem * item = currentItem ( ) ;
  return NotNull ( item )                  ;
}

bool N::ConditionLists::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return true                                                ;
}

bool N::ConditionLists::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::ConditionLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::ConditionLists::finishDrag(QMouseEvent * event)
{
  return true   ;
}

void N::ConditionLists::Configure(void)
{
  NewTreeWidgetItem            ( head                      ) ;
  ////////////////////////////////////////////////////////////
  head -> setText              ( 0 , tr("Name")            ) ;
  head -> setText              ( 1 , tr("Type")            ) ;
  head -> setText              ( 2 , tr("Identifier")      ) ;
  head -> setText              ( 3 , tr("Value" )          ) ;
  head -> setText              ( 4 , tr("Indicator")       ) ;
  head -> setText              ( 5 , tr("Membership")      ) ;
  head -> setText              ( 6 , ""                    ) ;
  setWindowTitle               (     tr("Condition lists") ) ;
  ////////////////////////////////////////////////////////////
  setFocusPolicy               ( Qt::WheelFocus            ) ;
  setDragDropMode              ( DragOnly                  ) ;
  setRootIsDecorated           ( false                     ) ;
  setAlternatingRowColors      ( true                      ) ;
  setSelectionMode             ( SingleSelection           ) ;
  setColumnCount               ( 7                         ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded     ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded     ) ;
  setAllAlignments             ( head , Qt::AlignCenter    ) ;
  setFont                      ( head , Fonts::ListView    ) ;
  assignHeaderItems            ( head                      ) ;
  MountClicked                 ( 2                         ) ;
  ////////////////////////////////////////////////////////////
  TypeNames [ Decisions::Node        ] = tr("Node")          ;
  TypeNames [ Decisions::Database    ] = tr("Database")      ;
  TypeNames [ Decisions::File        ] = tr("File")          ;
  TypeNames [ Decisions::Opportunity ] = tr("Opportunity")   ;
  TypeNames [ Decisions::Probability ] = tr("Probability")   ;
  TypeNames [ Decisions::Logic       ] = tr("Logic")         ;
  TypeNames [ Decisions::Fuzzy       ] = tr("Fuzzy")         ;
  TypeNames [ Decisions::Dynamic     ] = tr("Dynamic")       ;
  TypeNames [ Decisions::Functional  ] = tr("Functional")    ;
  TypeNames [ Decisions::Script      ] = tr("Script")        ;
  TypeNames [ Decisions::Virtual     ] = tr("Virtual")       ;
  ////////////////////////////////////////////////////////////
  setAlignment ( 3 , Qt::AlignVCenter | Qt::AlignRight )     ;
  setAlignment ( 5 , Qt::AlignVCenter | Qt::AlignRight )     ;
  ////////////////////////////////////////////////////////////
  plan -> setFont              ( this                      ) ;
  PassDragDrop = false                                       ;
}

bool N::ConditionLists::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Rename       , Rename      ( ) ) ;
  LinkAction        ( Copy         , Copy        ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  LinkAction        ( Paste        , Paste       ( ) ) ;
  LinkAction        ( Export       , Export      ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone  ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll   ( ) ) ;
  LinkAction        ( Language     , Language    ( ) ) ;
  LinkAction        ( Font         , setFont     ( ) ) ;
  if                ( ! isStandby ( )                ) {
    LinkAction      ( Delete       , Delete      ( ) ) ;
  }                                                    ;
  return true                                          ;
}

void N::ConditionLists::run(int T,ThreadData * d)
{
  nDropOut         ( ! IsContinue ( d ) ) ;
  switch           ( T                  ) {
    case 10001                            :
      startLoading (                    ) ;
      Load         ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
  }                                       ;
}

bool N::ConditionLists::Load(ThreadData * d)
{
  WaitClear     (                  )                                         ;
  emit OnBusy   (                  )                                         ;
  EnterSQL      ( SC , plan -> sql )                                         ;
    QString Q                                                                ;
    QString name                                                             ;
    SUID    uuid                                                             ;
    UUIDs   Uuids                                                            ;
    //////////////////////////////////////////////////////////////////////////
    if ( isStandby ( ) )                                                     {
      Uuids = SC . Uuids                                                     (
                PlanTable(Conditions)                                        ,
                "uuid"                                                       ,
                SC.OrderBy   ( "id"       , Sorting )                      ) ;
    } else                                                                   {
      Uuids = Subordination                                                  (
                SC                                                           ,
                ObjectUuid ( )                                               ,
                ObjectType ( )                                               ,
                Types::Condition                                             ,
                Connexion  ( )                                               ,
                SC . OrderBy ( "position" , Sorting )                      ) ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    foreach (uuid,Uuids)                                                     {
      NewTreeWidgetItem ( IT )                                               ;
      name = SC . getName ( PlanTable(Names),"uuid",vLanguageId,uuid )       ;
      IT -> setData   ( 0,Qt::UserRole,uuid      )                           ;
      IT -> setText   ( 0,name                   )                           ;
      Q = SC .sql . SelectFrom                                               (
            "type,identifier,value,indicator,membership"                     ,
            PlanTable(Conditions)                                            ,
            SC.WhereUuid(uuid)                                             ) ;
      if ( SC.Fetch(Q) )                                                     {
        int     t     = SC . Int    ( 0 )                                    ;
        QString ident = SC . String ( 1 )                                    ;
        int     v     = SC . Int    ( 2 )                                    ;
        SUID    indi  = SC . Uuid   ( 3 )                                    ;
        double  mems  = SC . Double ( 4 )                                    ;
        QString indn  = ""                                                   ;
        //////////////////////////////////////////////////////////////////////
        if ( indi > 0 )                                                      {
          indn = SC . getName ( PlanTable(Names)                             ,
                                "uuid"                                       ,
                                vLanguageId                                  ,
                                indi                                       ) ;
        }                                                                    ;
        //////////////////////////////////////////////////////////////////////
        IT -> setData ( 1 , Qt::UserRole , t                               ) ;
        IT -> setText ( 1 , TypeNames [ t ]                                ) ;
        IT -> setText ( 2 , ident                                          ) ;
        IT -> setText ( 3 , QString::number(v)                             ) ;
        IT -> setText ( 4 , indn                                           ) ;
        IT -> setText ( 5 , QString("%1").arg(mems,0,'f',6,QChar('0'))     ) ;
      }                                                                      ;
      setAlignments   ( IT                                                 ) ;
      addTopLevelItem ( IT                                                 ) ;
    }                                                                        ;
  LeaveSQL      ( SC , plan->sql )                                           ;
  emit GoRelax  (                )                                           ;
  emit AutoFit  (                )                                           ;
  Alert         ( Done           )                                           ;
  return true                                                                ;
}

bool N::ConditionLists::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

void N::ConditionLists::List(void)
{
  startup ( ) ;
}

void N::ConditionLists::Copy(void)
{
  QMimeData * mime = dragMime    (      ) ;
  if (IsNull(mime)) return                ;
  qApp->clipboard()->setMimeData ( mime ) ;
}

void N::ConditionLists::Insert(void)
{
  NewTreeWidgetItem ( IT                   ) ;
  IT -> setData     ( 0 , Qt::UserRole , 0 ) ;
  setAlignments     ( IT                   ) ;
  addTopLevelItem   ( IT                   ) ;
  scrollToItem      ( IT                   ) ;
  doubleClicked     ( IT , 0               ) ;
}

void N::ConditionLists::Rename(void)
{
  QTreeWidgetItem * IT        ;
  IT = currentItem ( )        ;
  if ( IsNull ( IT ) ) return ;
  doubleClicked ( IT , 0 )    ;
}

void N::ConditionLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit      * line                              ;
  QComboBox      * list                              ;
  QSpinBox       * spin                              ;
  QDoubleSpinBox * dbts                              ;
  ////////////////////////////////////////////////////
  removeOldItem ( )                                  ;
  ////////////////////////////////////////////////////
  switch ( column )                                  {
    case 0                                           : // Name
      line = setLineEdit                             (
               item                                  ,
               column                                ,
               SIGNAL(editingFinished())             ,
               SLOT  (NameFinished   ())           ) ;
      line->setFocus(Qt::TabFocusReason)             ;
    break                                            ;
    case 1                                           : // Type
      list = setComboBox                             (
               item                                  ,
               column                                ,
               SIGNAL(activated  (int))              ,
               SLOT  (typeChanged(int))            ) ;
      for (int i = Decisions::Node                   ;
               i < Decisions::EndOfConditions ; i++) {
        list -> addItem ( TypeNames [ i ] , i      ) ;
      }                                              ;
      if ( NotNull(item) )                           {
        int ix = nTreeInt ( item , column )          ;
        list -> setCurrentIndex ( ix )               ;
      }                                              ;
      list -> setEditable ( true )                   ;
      list -> showPopup   (      )                   ;
    break                                            ;
    case 2                                           : // Identifier
      line = setLineEdit                             (
               item                                  ,
               column                                ,
               SIGNAL(editingFinished())             ,
               SLOT  (identFinished  ())           ) ;
      line->setFocus(Qt::TabFocusReason)             ;
    break                                            ;
    case 3                                           : // Value
      spin = setSpinBox                              (
               item                                  ,
               column                                ,
               INT32_MIN                             ,
               INT32_MAX                             ,
               SIGNAL(editingFinished())             ,
               SLOT  (valueModified  ())           ) ;
    break                                            ;
    case 4                                           : // Indicator
      indicatorClicked ( item , column )             ;
    break                                            ;
    case 5                                           : // Membership
      dbts = setDoubleSpin                           (
               item                                  ,
               column                                ,
               -10000000000.0f                       ,
                10000000000.0f                       ,
               SIGNAL(editingFinished())             ,
               SLOT  (memberModified ())           ) ;
    break                                            ;
  }                                                  ;
}

SUID N::ConditionLists::assureItem (
        SqlConnection & SC         ,
        SUID            u          ,
        QString         name       )
{
  if ( u <= 0 )                                                            {
    u  = SC . Unique ( PlanTable(MajorUuid ) , "uuid" , 975321           ) ;
    SC . assureUuid  ( PlanTable(MajorUuid ) , u      , Types::Condition ) ;
    SC . assureUuid  ( PlanTable(Conditions) , u      , 0                ) ;
    if ( ( u > 0 ) && ( ! isStandby ( ) ) )                                {
      GroupItems::Join                                                     (
        SC                                                                 ,
        ObjectUuid ( )                                                     ,
        ObjectType ( )                                                     ,
        Types::Condition                                                   ,
        Connexion  ( )                                                     ,
        0                                                                  ,
        u                                                                ) ;
    }                                                                      ;
  }                                                                        ;
  if ( u > 0 )                                                             {
    SC . assureName  ( PlanTable(Names) , u , vLanguageId , name         ) ;
  }                                                                        ;
  return u                                                                 ;
}

void N::ConditionLists::NameFinished(void)
{
  SUID        uuid = nTreeUuid ( ItemEditing , 0          ) ;
  QLineEdit * name = Casting   ( QLineEdit   , ItemWidget ) ;
  ///////////////////////////////////////////////////////////
  if (IsNull(name))                                         {
    removeOldItem ( )                                       ;
    return                                                  ;
  }                                                         ;
  QString action = name->text()                             ;
  if ( action . length ( ) <= 0 )                           {
    removeOldItem ( )                                       ;
    return                                                  ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  EnterSQL                   ( SC , plan->sql             ) ;
    uuid = assureItem        ( SC, uuid , action          ) ;
    if ( uuid > 0 )                                         {
      ItemEditing -> setData ( 0    , Qt::UserRole , uuid ) ;
      ItemEditing -> setText ( 0    , action              ) ;
      emit Update            ( this , uuid         , 0    ) ;
    }                                                       ;
  LeaveSQL ( SC , plan->sql )                               ;
  ///////////////////////////////////////////////////////////
  removeOldItem (      )                                    ;
  Alert         ( Done )                                    ;
}

void N::ConditionLists::typeChanged(int)
{
  SUID        uuid = nTreeUuid ( ItemEditing , 0          )                  ;
  QComboBox * comb = Casting   ( QComboBox   , ItemWidget )                  ;
  if (IsNull(comb))                                                          {
    removeOldItem ( )                                                        ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  EnterSQL      ( SC , plan->sql )                                           ;
    QString Q                                                                ;
    QString n = comb -> currentText  ( )                                     ;
    int     t = comb -> currentIndex ( )                                     ;
    //////////////////////////////////////////////////////////////////////////
    ItemEditing -> setData ( ItemColumn , Qt::UserRole , t )                 ;
    ItemEditing -> setText ( ItemColumn , n                )                 ;
    //////////////////////////////////////////////////////////////////////////
    Q = SC . sql . Update                                                    (
          PlanTable(Conditions)                                              ,
          SC . sql . Where ( 1 , "uuid" )                                    ,
          1                                                                  ,
          "type"                                                           ) ;
    SC . Prepare ( Q             )                                           ;
    SC . Bind    ( "uuid" , uuid )                                           ;
    SC . Bind    ( "type" , t    )                                           ;
    SC . Exec    (               )                                           ;
    //////////////////////////////////////////////////////////////////////////
  ErrorSQL      ( SC , plan->sql )                                           ;
  LeaveSQL      ( SC , plan->sql )                                           ;
  ////////////////////////////////////////////////////////////////////////////
  removeOldItem (                )                                           ;
  Alert         ( Done           )                                           ;
}

void N::ConditionLists::identFinished(void)
{
  SUID        uuid = nTreeUuid ( ItemEditing , 0          )                  ;
  QLineEdit * line = Casting   ( QLineEdit   , ItemWidget )                  ;
  if ( IsNull(line) )                                                        {
    removeOldItem ( )                                                        ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  EnterSQL       ( SC , plan->sql                )                           ;
    QString Q                                                                ;
    QString n = line -> text ( )                                             ;
    ItemEditing -> setText ( ItemColumn , n )                                ;
    //////////////////////////////////////////////////////////////////////////
    Q = SC . sql . Update                                                    (
          PlanTable(Conditions)                                              ,
          SC . sql . Where ( 1 , "uuid" )                                    ,
          1                                                                  ,
          "identifier"                                                     ) ;
    SC . Prepare ( Q                             )                           ;
    SC . Bind    ( "uuid"       , uuid           )                           ;
    SC . Bind    ( "identifier" , n . toUtf8 ( ) )                           ;
    SC . Exec    (                               )                           ;
    //////////////////////////////////////////////////////////////////////////
  ErrorSQL       ( SC , plan->sql                )                           ;
  LeaveSQL       ( SC , plan->sql                )                           ;
  ////////////////////////////////////////////////////////////////////////////
  removeOldItem  (                               )                           ;
  Alert          ( Done                          )                           ;
}

void N::ConditionLists::valueModified(void)
{
  SUID       uuid = nTreeUuid ( ItemEditing , 0          )                   ;
  QSpinBox * spin = Casting   ( QSpinBox    , ItemWidget )                   ;
  if ( IsNull(spin) )                                                        {
    removeOldItem ( )                                                        ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  EnterSQL       ( SC , plan->sql )                                          ;
    QString Q                                                                ;
    int     n = spin -> value ( )                                            ;
    ItemEditing -> setText ( ItemColumn , QString::number(n) )               ;
    //////////////////////////////////////////////////////////////////////////
    Q = SC . sql . Update                                                    (
          PlanTable(Conditions)                                              ,
          SC . sql . Where ( 1 , "uuid" )                                    ,
          1                                                                  ,
          "value"                                                          ) ;
    SC . Prepare ( Q              )                                          ;
    SC . Bind    ( "uuid"  , uuid )                                          ;
    SC . Bind    ( "value" , n    )                                          ;
    SC . Exec    (                )                                          ;
    //////////////////////////////////////////////////////////////////////////
  ErrorSQL       ( SC , plan->sql )                                          ;
  LeaveSQL       ( SC , plan->sql )                                          ;
  ////////////////////////////////////////////////////////////////////////////
  removeOldItem  (                )                                          ;
  Alert          ( Done           )                                          ;
}

void N::ConditionLists::indicatorClicked(QTreeWidgetItem *item, int column)
{
}

void N::ConditionLists::memberModified(void)
{
  SUID             uuid = nTreeUuid ( ItemEditing    , 0          )          ;
  QDoubleSpinBox * dbts = Casting   ( QDoubleSpinBox , ItemWidget )          ;
  if ( IsNull(dbts) )                                                        {
    removeOldItem ( )                                                        ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  EnterSQL      ( SC , plan->sql )                                           ;
    QString Q                                                                ;
    double  n = dbts -> value ( )                                            ;
    QString m = QString("%1").arg(n,0,'f',6,QChar('0'))                      ;
    ItemEditing -> setText ( ItemColumn , m )                                ;
    //////////////////////////////////////////////////////////////////////////
    Q = SC . sql . Update                                                    (
          PlanTable(Conditions)                                              ,
          SC . sql . Where ( 1 , "uuid" )                                    ,
          1                                                                  ,
          "membership"                                                     ) ;
    SC . Prepare ( Q                   )                                     ;
    SC . Bind    ( "uuid"       , uuid )                                     ;
    SC . Bind    ( "membership" , n    )                                     ;
    SC . Exec    (                     )                                     ;
    //////////////////////////////////////////////////////////////////////////
  ErrorSQL      ( SC , plan->sql )                                           ;
  LeaveSQL      ( SC , plan->sql )                                           ;
  ////////////////////////////////////////////////////////////////////////////
  removeOldItem (                )                                           ;
  Alert         ( Done           )                                           ;
}

void N::ConditionLists::Delete(void)
{
  if ( isStandby ( ) ) return ;
}

void N::ConditionLists::Paste(void)
{
  Paste ( nClipboardText ) ;
}

void N::ConditionLists::Paste(QString text)
{
  QStringList L = text . split ( '\n' )               ;
  if ( L . count ( ) <= 0 ) return                    ;
  EnterSQL                  ( SC , plan->sql        ) ;
    QString n                                         ;
    SUID    u                                         ;
    foreach                 ( n , L                 ) {
      NewTreeWidgetItem     ( it                    ) ;
      u   = assureItem      ( SC , 0            , n ) ;
      it -> setData         ( 0  , Qt::UserRole , u ) ;
      it -> setData         ( 1  , Qt::UserRole , u ) ;
      it -> setText         ( 0  , n                ) ;
      addTopLevelItem       ( it                    ) ;
      plan -> processEvents (                       ) ;
    }                                                 ;
  ErrorSQL                  ( SC , plan->sql        ) ;
  LeaveSQL                  ( SC , plan->sql        ) ;
  emit AutoFit              (                       ) ;
  Alert                     ( Done                  ) ;
}

void N::ConditionLists::Export(void)
{
  QString filename = QFileDialog::getSaveFileName (
                       this                       ,
                       tr("Export conditions")    ,
                       plan->Temporary("")        ,
                       "*.*\n"
                       "*.txt"                  ) ;
  nDropOut ( filename.length() <= 0 )             ;
  /////////////////////////////////////////////////
  QString T = toText ( )                          ;
  if ( T.length() <= 0 ) return                   ;
  File::toFile ( filename , T )                   ;
}

bool N::ConditionLists::Menu(QPoint)
{
  nScopedMenu ( mm , this )                                   ;
  QTreeWidgetItem * header = headerItem  ( )                  ;
  QTreeWidgetItem * IT     = currentItem ( )                  ;
  QMenu           * columns                                   ;
  QAction         * aa                                        ;
  int               id                                        ;
  /////////////////////////////////////////////////////////////
  if ( NotNull ( IT ) ) mm . add ( 101 , tr("Edit") )         ;
  mm . add ( 102 , tr("Refresh") )                            ;
  columns = mm . addMenu ( tr("Columns") )                    ;
  DockingMenu    ( mm   )                                     ;
  /////////////////////////////////////////////////////////////
  for (int i = 2 ; i < columnCount ( ) ; i++ )                {
    mm . add                                                  (
      columns                                                 ,
      100000 + i - 1                                          ,
      header->text(i-1)                                       ,
      true                                                    ,
      ! isColumnHidden(i-1)                                 ) ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  mm . setFont   ( plan )                                     ;
  /////////////////////////////////////////////////////////////
  aa = mm . exec (      )                                     ;
  if ( IsNull     ( aa      ) ) return true                   ;
  if ( RunDocking ( mm , aa ) ) return true                   ;
  /////////////////////////////////////////////////////////////
  id = mm [ aa ]                                              ;
  if ( ( id > 100000 ) && ( id < 100020 ) )                   {
    id -= 100000                                              ;
    setColumnHidden ( id , ! aa -> isChecked ( ) )            ;
    return true                                               ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  switch ( id )                                               {
    case 101                                                  :
      if (NotNull(IT))                                        {
        SUID uuid = nTreeUuid ( IT , 0 )                      ;
        if (uuid>0)                                           {
          int Type = nTreeInt(IT,1)                           ;
          emit Edit ( this , IT -> text ( 0 ) , uuid , Type ) ;
        }                                                     ;
      }                                                       ;
    break                                                     ;
    case 102                                                  :
      startup ( )                                             ;
    break                                                     ;
    default                                                   :
    break                                                     ;
  }                                                           ;
  return true                                                 ;
}
