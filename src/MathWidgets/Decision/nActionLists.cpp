#include <mathwidgets.h>

N::ActionLists:: ActionLists ( QWidget * parent , Plan * p             )
               : TreeDock    (           parent ,        p             )
               , Ownership   ( 0 , Types::None , Groups::Subordination )
               , GroupItems  (                           p             )
               , Sorting     ( Qt::AscendingOrder                      )
               , dropAction  ( false                                   )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ActionLists::~ActionLists(void)
{
}

QSize N::ActionLists::sizeHint(void) const
{
  return SizeSuggestion ( QSize ( 480 , 320 ) ) ;
}

QMimeData * N::ActionLists::dragMime (void)
{
  QTreeWidgetItem * IT = currentItem()     ;
  if (IsNull(IT)) return NULL              ;
  SUID  uuid  = nTreeUuid(IT,0)            ;
  QMimeData * mime = new QMimeData()       ;
  setMime ( mime , "action/uuid"  , uuid ) ;
  return mime                              ;
}

bool N::ActionLists::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::ActionLists::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return true                                                ;
}

bool N::ActionLists::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::ActionLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::ActionLists::finishDrag(QMouseEvent * event)
{
  return true   ;
}

void N::ActionLists::Configure(void)
{
  NewTreeWidgetItem            ( head                   ) ;
  /////////////////////////////////////////////////////////
  head -> setText              ( 0 , tr("Action")       ) ;
  head -> setText              ( 1 , tr("Type"  )       ) ;
  head -> setText              ( 2 , ""                 ) ;
  setWindowTitle               ( tr("Action lists")     ) ;
  /////////////////////////////////////////////////////////
  setFocusPolicy               ( Qt::WheelFocus         ) ;
  setDragDropMode              ( DragOnly               ) ;
  setRootIsDecorated           ( false                  ) ;
  setAlternatingRowColors      ( true                   ) ;
  setSelectionMode             ( SingleSelection        ) ;
  setColumnCount               ( 3                      ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded  ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded  ) ;
  /////////////////////////////////////////////////////////
  setAllAlignments             ( head , Qt::AlignCenter ) ;
  setFont                      ( head , Fonts::ListView ) ;
  assignHeaderItems            ( head                   ) ;
  /////////////////////////////////////////////////////////
  MountClicked                 ( 2                      ) ;
  plan -> setFont              ( this                   ) ;
  PassDragDrop = false                                    ;
}

bool N::ActionLists::FocusIn(void)
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
  if ( ! isStandby  (                              ) ) {
    LinkAction      ( Delete       , Delete      ( ) ) ;
  }                                                    ;
  return true                                          ;
}

void N::ActionLists::run(int T,ThreadData * d)
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

bool N::ActionLists::Load(ThreadData * d)
{
  WaitClear (                  )                         ;
  EnterSQL  ( SC , plan -> sql )                         ;
    QString Q                                            ;
    QString name                                         ;
    SUID    uuid                                         ;
    SUID    tuid                                         ;
    UUIDs   Uuids                                        ;
    //////////////////////////////////////////////////////
    if ( isStandby ( ) )                                 {
      Uuids = SC . Uuids                                 (
                PlanTable(Actions)                       ,
                "uuid"                                   ,
                SC.OrderBy   ( "id"       , Sorting )  ) ;
    } else                                               {
      Uuids = Subordination                              (
                SC                                       ,
                ObjectUuid ( )                           ,
                ObjectType ( )                           ,
                Types::Action                            ,
                Connexion  ( )                           ,
                SC . OrderBy ( "position" , Sorting )  ) ;
    }                                                    ;
    //////////////////////////////////////////////////////
    Tnames [ 0 ] = tr("Undecided")                       ;
    foreach (uuid,Uuids)                                 {
      NewTreeWidgetItem ( IT )                           ;
      name = SC . getName                                (
               PlanTable(Names)                          ,
               "uuid"                                    ,
               vLanguageId                               ,
               uuid                                    ) ;
      IT -> setData   ( 0 , Qt::UserRole , uuid        ) ;
      IT -> setData   ( 1 , Qt::UserRole , 0           ) ;
      IT -> setText   ( 0 , name                       ) ;
      IT -> setText   ( 1 , Tnames [ 0 ]               ) ;
      Q = SC . sql . SelectFrom                          (
            "type"                                       ,
            PlanTable(Actions)                           ,
            SC . WhereUuid ( uuid )                    ) ;
      if (SC.Fetch(Q))                                   {
        tuid = SC . Uuid ( 0 )                           ;
        if ( ! Tnames . contains ( tuid ) )              {
          Tnames[tuid] = SC . getName                    (
                           PlanTable(Names)              ,
                           "uuid"                        ,
                           vLanguageId                   ,
                           tuid                        ) ;
        }                                                ;
        IT -> setData   ( 1 , Qt::UserRole , (int)tuid ) ;
        IT -> setText   ( 1 , Tnames[tuid]             ) ;
      }                                                  ;
      addTopLevelItem   ( IT                           ) ;
    }                                                    ;
  LeaveSQL              ( SC , plan->sql               ) ;
  emit AutoFit          (                              ) ;
  Alert                 ( Done                         ) ;
  return true                                            ;
}

bool N::ActionLists::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

void N::ActionLists::List(void)
{
  startup ( ) ;
}

void N::ActionLists::Copy(void)
{
  QMimeData * mime = dragMime    (      ) ;
  if (IsNull(mime)) return                ;
  qApp->clipboard()->setMimeData ( mime ) ;
}

void N::ActionLists::Insert(void)
{
  NewTreeWidgetItem ( IT                   ) ;
  IT -> setData     ( 0 , Qt::UserRole , 0 ) ;
  IT -> setData     ( 1 , Qt::UserRole , 0 ) ;
  setAlignments     ( IT                   ) ;
  addTopLevelItem   ( IT                   ) ;
  scrollToItem      ( IT                   ) ;
  doubleClicked     ( IT , 0               ) ;
}

void N::ActionLists::Rename(void)
{
  QTreeWidgetItem * IT        ;
  IT = currentItem ( )        ;
  if ( IsNull ( IT ) ) return ;
  doubleClicked ( IT , 0 )    ;
}

void N::ActionLists::doubleClicked(QTreeWidgetItem * item, int column)
{
  nDropOut ( IsNull(item) )              ;
  QLineEdit * le = NULL                  ;
  switch ( column )                      {
    case 0                               :
      le = setLineEdit                   (
             item                        ,
             0                           ,
             SIGNAL(editingFinished())   ,
             SLOT  (NameFinished   ()) ) ;
      le->setFocus(Qt::TabFocusReason)   ;
    break                                ;
    case 1                               :
      changeType(item)                   ;
    break                                ;
  }                                      ;
}

SUID N::ActionLists::assureItem (
        SqlConnection & SC      ,
        SUID            u       ,
        QString         name    )
{
  if ( u <= 0 )                                                        {
    u  = SC . Unique ( PlanTable(MajorUuid) , "uuid" , 312975        ) ;
    SC . assureUuid  ( PlanTable(MajorUuid) , u      , Types::Action ) ;
    SC . assureUuid  ( PlanTable(Actions  ) , u      , Types::None   ) ;
    if ( ( u > 0 ) && ( ! isStandby ( ) ) )                            {
      GroupItems::Join                                                 (
        SC                                                             ,
        ObjectUuid ( )                                                 ,
        ObjectType ( )                                                 ,
        Types::Action                                                  ,
        Connexion  ( )                                                 ,
        0                                                              ,
        u                                                            ) ;
    }                                                                  ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  if ( u > 0 )                                                         {
    SC . assureName  ( PlanTable(Names) , u , vLanguageId , name     ) ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  return u                                                             ;
}

void N::ActionLists::NameFinished(void)
{
  SUID        uuid = nTreeUuid  ( ItemEditing , ItemColumn ) ;
  QLineEdit * name = Casting    ( QLineEdit   , ItemWidget ) ;
  if (IsNull(name))                                          {
    removeOldItem               (                          ) ;
    return                                                   ;
  }                                                          ;
  QString action = name -> text (                          ) ;
  if                            ( action . length ( ) <= 0 ) {
    removeOldItem               (                          ) ;
    return                                                   ;
  }                                                          ;
  EnterSQL                      ( SC , plan->sql           ) ;
    uuid = assureItem           ( SC , uuid , action       ) ;
    if                          ( uuid > 0                 ) {
      int Aype = nTreeInt       ( ItemEditing , 1          ) ;
      ItemEditing -> setData    ( 0 , Qt::UserRole , uuid  ) ;
      ItemEditing -> setData    ( 1 , Qt::UserRole , Aype  ) ;
      ItemEditing -> setText    ( 0 , action               ) ;
      ItemEditing -> setText    ( 1 , Tnames [ Aype ]      ) ;
      emit Update               ( this , uuid , Aype       ) ;
    }                                                        ;
  LeaveSQL                      ( SC , plan->sql           ) ;
  removeOldItem                 (                          ) ;
  Alert                         ( Done                     ) ;
}

void N::ActionLists::changeType(QTreeWidgetItem * item)
{
  nDropOut ( IsNull(item) )                ;
  UUIDs      Uuids                         ;
  SUID       tuid  = nTreeUuid(item,1)     ;
  ComboBox * combo = NULL                  ;
  combo = (ComboBox *)setComboBox          (
            item                           ,
            1                              ,
            SIGNAL ( activated    (int))   ,
            SLOT   ( typeFinished (int)) ) ;
  for (int i=0;i<Types::TypesEnd;i++)      {
    Uuids << (SUID) i                      ;
  }                                        ;
  combo -> addItems        ( Uuids       ) ;
  combo -> blockSignals    ( true        ) ;
  combo -> setCurrentIndex ( (int)tuid   ) ;
  combo -> blockSignals    ( false       ) ;
  combo -> showPopup       (             ) ;
}

void N::ActionLists::typeFinished(int index)
{
  if (index>=0)                                                {
    SUID        tuid  = index                                  ;
    SUID        u     = nTreeUuid ( ItemEditing , 0          ) ;
    QComboBox * combo = Casting   ( QComboBox   , ItemWidget ) ;
    if ( NotNull(combo) && ( u > 0 ) && ( tuid > 0 ) )         {
      QString name = combo -> currentText ( )                  ;
      SqlConnection SC ( plan -> sql )                         ;
      if ( SC . open("ActionLists","typeFinished") )           {
        QString Q                                              ;
        Q = SC . sql . Update                                  (
              PlanTable(Actions)                               ,
              SC . sql . Where ( 1 , "uuid"                  ) ,
              1                                                ,
              "type"                                         ) ;
        SC . Prepare           ( Q                           ) ;
        SC . Bind              ( "uuid" , u                  ) ;
        SC . Bind              ( "type" , tuid               ) ;
        SC . Exec              (                             ) ;
        ItemEditing -> setData ( 1 , Qt::UserRole , tuid     ) ;
        ItemEditing -> setText ( 1 , name                    ) ;
        Alert                  ( Done                        ) ;
        SC . close             (                             ) ;
      } else                                                   {
        Alert                  ( Error                       ) ;
        SC . close             (                             ) ;
      }                                                        ;
      SC . remove              (                             ) ;
    }                                                          ;
  }                                                            ;
  removeOldItem                (                             ) ;
  Alert                        ( Done                        ) ;
}

void N::ActionLists::Delete(void)
{
  if ( isStandby ( ) ) return ;
}

void N::ActionLists::Paste(void)
{
  Paste ( nClipboardText ) ;
}

void N::ActionLists::Paste(QString text)
{
  QStringList L = text . split ( '\n'                  ) ;
  if ( L . count ( ) <= 0 ) return                       ;
  EnterSQL                     ( SC , plan->sql        ) ;
    QString n                                            ;
    SUID    u                                            ;
    foreach                    ( n , L                 ) {
      NewTreeWidgetItem        ( it                    ) ;
      u = assureItem           ( SC , 0            , n ) ;
      it -> setData            ( 0  , Qt::UserRole , u ) ;
      it -> setData            ( 1  , Qt::UserRole , 0 ) ;
      it -> setText            ( 0  , n                ) ;
      it -> setText            ( 1  , Tnames [ 0 ]     ) ;
      addTopLevelItem          ( it                    ) ;
      plan -> processEvents    (                       ) ;
    }                                                    ;
  ErrorSQL                     ( SC , plan->sql        ) ;
  LeaveSQL                     ( SC , plan->sql        ) ;
  Alert                        ( Done                  ) ;
}

void N::ActionLists::Export(void)
{
  QString filename = QFileDialog::getSaveFileName (
                       this                       ,
                       tr("Export actions")       ,
                       plan->Temporary("")        ,
                       "*.*\n"
                       "*.txt"                  ) ;
  nDropOut ( filename.length() <= 0 )             ;
  /////////////////////////////////////////////////
  QString T = toText ( )                          ;
  if ( T . length ( ) <= 0 ) return               ;
  File::toFile ( filename , T )                   ;
}

bool N::ActionLists::Menu(QPoint)
{
  nScopedMenu ( mm , this )                                   ;
  QTreeWidgetItem * IT = currentItem ( )                      ;
  QAction         * aa                                        ;
  if ( NotNull(IT) )                                          {
    mm . add         ( 101 , tr("Edit")    )                  ;
  }                                                           ;
  mm  . add          ( 102 , tr("Refresh") )                  ;
  mm  . addSeparator (                     )                  ;
  aa  = mm . add     ( 901 , tr("Type")    )                  ;
  aa -> setCheckable ( true                )                  ;
  aa -> setChecked   (! isColumnHidden(1)  )                  ;
  /////////////////////////////////////////////////////////////
  DockingMenu     ( mm        )                               ;
  mm . setFont    ( plan      )                               ;
  aa = mm . exec  (           )                               ;
  if ( IsNull     ( aa      ) ) return true                   ;
  if ( RunDocking ( mm , aa ) ) return true                   ;
  /////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                        {
    case     101                                              :
      if (NotNull(IT))                                        {
        SUID uuid = nTreeUuid ( IT , 0 )                      ;
        if (uuid>0)                                           {
          int Type = nTreeInt ( IT , 1 )                      ;
          emit Edit ( this , IT -> text ( 0 ) , uuid , Type ) ;
        }                                                     ;
      }                                                       ;
    break                                                     ;
    case 102                                                  :
      List ( )                                                ;
    break                                                     ;
    case 901                                                  :
      setColumnHidden ( 1 , !aa->isChecked() )                ;
    break                                                     ;
    default                                                   :
    break                                                     ;
  }                                                           ;
  return true                                                 ;
}
