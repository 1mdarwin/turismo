<?php

/**
 * @file
 * A basic example of using the Migrate module to import taxonomy, users, nodes,
 * and comments.
 *
 * The basic idea is
 * - The users in the source application are listed in the
 *   migrate_example_beer_account table and are transformed into Drupal users.
 * - Drupal "beer" nodes describe beers; The information to create the nodes
 *   comes from the migrate_example_beer_node table.
 * - Taxonomy terms for the beer nodes (ale, pilsner) come from the
 *   migrate_example_beer_topic table and they are applied to nodes using the
 *   source information in the migrate_example_beer_topic_node table.
 * - Comments to be attached to the beer nodes are described in the source
 *   migrate_example_beer_comment table.
 *
 * We will use the Migrate API to import and transform this data and turn it into
 * a working Drupal system.
 */

/**
 * To define a migration process from a set of source data to a particular
 * kind of Drupal object (for example, a specific node type), you define
 * a class derived from Migration. You must define a constructor to initialize
 * your migration object. By default, your class name will be the "machine name"
 * of the migration, by which you refer to it. Note that the machine name is
 * case-sensitive.
 *
 * In any serious migration project, you will find there are some options
 * which are common to the individual migrations you're implementing. You can
 * define an abstract intermediate class derived from Migration, then derive your
 * individual migrations from that, to share settings, utility functions, etc.
 */
abstract class CatastroMigration extends DynamicMigration {
  public function __construct() {
    // Always call the parent constructor first for basic setup
    parent::__construct();

    // With migrate_ui enabled, migration pages will indicate people involved in
    // the particular migration, with their role and contact info. We default the
    // list in the shared class; it can be overridden for specific migrations.
    $this->team = array(
      new MigrateTeamMember('Darwin Betancourt', 'dbetancourt@drwsoluciones.net', t('Product Owner')),
      new MigrateTeamMember('DRW SolucioneS', 'info@drwsoluciones.net', t('Implementor')),
    );

    // Individual mappings in a migration can be linked to a ticket or issue
    // in an external tracking system. Define the URL pattern here in the shared
    // class with ':id:' representing the position of the issue number, then add
    // ->issueNumber(1234) to a mapping.
    $this->issuePattern = 'http://drupal.org/node/:id:';
  }
}

/**
 * There are four essential components to set up in your constructor:
 *  $this->source - An instance of a class derived from MigrateSource, this
 *    will feed data to the migration.
 *  $this->destination - An instance of a class derived from MigrateDestination,
 *    this will receive data that originated from the source and has been mapped
 *    by the Migration class, and create Drupal objects.
 *  $this->map - An instance of a class derived from MigrateMap, this will keep
 *    track of which source items have been imported and what destination objects
 *    they map to.
 *  Mappings - Use $this->addFieldMapping to tell the Migration class what source
 *    fields correspond to what destination fields, and additional information
 *    associated with the mappings.
 */


/**
 * The BeerNodeMigration uses the migrate_example_beer_node table as source
 * and creates Drupal nodes of type 'Beer' as destination.
 */
class HostalNodeMigration extends CatastroMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Hostal of the Loja');

    $path_module=drupal_get_path('module', 'migrate_catastro');
    //$csv_path="/home/drw/Linux/work/turismo/".$path_module."/csv/catastro2011.csv";
    $csv_path=$path_module."/csv/catastro2011.csv";

    $columns = array(
      0 => array('indice','Indice del Archivo'),
      1 => array('nombre','Nombre'),
      2 => array('direccion','Direccion'),
      3 => array('clase','Clase'),
      4 => array('telf','Telf'),
      5 => array('ext','Ext'),
      6 => array('mail','Mail'),
      7 => array('tipo','Tipo'),
      8 => array('categoria','Categoria'),
      9 => array('website','WebSite'),

      );

    //$this->source = new MigrateSourceCSV($csv_path, $columns, array('embedded_newlines' => TRUE, "track_changes" => TRUE));
    $this->source = new MigrateSourceCSV($csv_path, $columns);

    // You may optionally declare dependencies for your migration - other migrations
    // which should run first. In this case, terms assigned to our nodes and
    // the authors of the nodes should be migrated before the nodes themselves.

    ///$this->dependencies = array('BeerTerm', 'BeerUser');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'indice' => array(
          'type' => 'int',
          'not null' => TRUE,
          'description' => 'Service ID.',
          //'alias' => 'b',
        )
      ),
      MigrateDestinationNode::getKeySchema()
    );    
    
    // Set up our destination - nodes of type migrate_example_beer
    //$this->destination = new MigrateDestinationNode('migrate_example_beer');
    $this->destination = new MigrateDestinationNode('servicio_catastro');

    // Mapped fields

    global $user;

    $this->addFieldMapping('title', 'nombre');
    $this->addFieldMapping('field_calle_primaria', 'direccion');
    $this->addFieldMapping('field_clase', 'clase');
    $this->addFieldMapping('field_telf', 'telf');
    $this->addFieldMapping('field_fax', 'ext');
    $this->addFieldMapping('field_email', 'mail')->defaultValue("info@turismo.gob.ec");
    $this->addFieldMapping('field_num_casa')->defaultValue("00-00");
    $this->addFieldMapping('field_seccion', 'tipo');
    $this->addFieldMapping('field_categoria', 'categoria');
    $this->addFieldMapping('field_website', 'website')->defaultValue("www.turismo.gob.ec");

    $this->addFieldMapping('uid')->defaultValue(1);

    ///$this->addFieldMapping('field_categoria', 'categoria')->defaultValue("General");

    
  }

}